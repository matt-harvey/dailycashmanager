// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "setup_wizard.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "app.hpp"
#include "application.hpp"
#include "client_data.hpp"
#include "filename_validation.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "icon.xpm"
#include "locale.hpp"
#include "make_currencies.hpp"
#include "make_default_accounts.hpp"
#include "multi_account_panel.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_conv.hpp"
#include "string_flags.hpp"
#include "sizing.hpp"
#include "visibility.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/array_utilities.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dataview.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/gbsizer.h>
#include <wx/msgdlg.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/treelist.h>
#include <wx/validate.h>
#include <wx/variant.h>
#include <wx/window.h>
#include <wx/wizard.h>
#include <map>
#include <sstream>
#include <set>
#include <string>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::DatabaseTransaction;
using std::map;
using std::ostringstream;
using std::set;
using std::string;
using std::vector;

// For debugging
#include <jewel/log.hpp>
#include <iostream>
using std::endl;


namespace filesystem = boost::filesystem;

namespace phatbooks
{
namespace gui
{


namespace
{
	filesystem::path wx_to_boost_filepath
	(	wxString const& wx_directory,
		wxString const& wx_filename
	)
	{
		filesystem::path const directory =
			filesystem::path(wx_to_std8(wx_directory));
		filesystem::path const filename =
			filesystem::path(wx_to_std8(wx_filename));
		filesystem::path ret = directory;
		ret /= filename;
		return ret;
	}

	wxString with_extension(wxString const& s)
	{
		filesystem::path const path(wx_to_std8(s));
		if
		(	std8_to_wx(path.extension().string()) ==
			Application::filename_extension()
		)
		{
			return s;
		}
		return s + Application::filename_extension();
	}

	wxString without_extension(wxString const& s)
	{
		filesystem::path const path(wx_to_std8(s));
		if
		(	std8_to_wx(path.extension().string()) ==
			Application::filename_extension()
		)
		{
			if (path.extension() != path)
			{
				return std8_to_wx(path.stem().string());
			}
		}
		return s;
	}

	void make_default_augmented_accounts
	(	PhatbooksDatabaseConnection& dbc,
		vector<AugmentedAccount>& vec,
		account_type::AccountType p_account_type,
		Decimal::places_type precision
	)
	{
		vector<Account> accounts;
		make_default_accounts(dbc, accounts, p_account_type);
		Decimal const zero(0, precision);
		for (vector<Account>::size_type i = 0; i != accounts.size(); ++i)
		{
			AugmentedAccount const augmented_account(accounts[i], zero);
			vec.push_back(augmented_account);
		}
		return;
	}

}  // end anonymous namespace



/*** SetupWizard ***/


SetupWizard::SetupWizard
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizard
	(	0,
		wxID_ANY,
		Application::application_name() + wxString(" Setup Wizard"),
		wxBitmap(icon_xpm),  // TODO Put a proper image here
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection),
	m_filepath_page(0),
	m_balance_sheet_account_page(0),
	m_pl_account_page(0)
{
	JEWEL_ASSERT (!m_database_connection.is_valid());
	m_filepath_page = new FilepathPage(this, m_database_connection);
	m_balance_sheet_account_page = new AccountPage
	(	this,
		account_super_type::balance_sheet,
		m_database_connection
	);
	m_pl_account_page = new AccountPage
	(	this,
		account_super_type::pl,
		m_database_connection
	);
	render_account_pages();
	wxWizardPageSimple::Chain
	(	m_filepath_page,
		m_balance_sheet_account_page
	);
	wxWizardPageSimple::Chain
	(	m_balance_sheet_account_page,
		m_pl_account_page
	);
	GetPageAreaSizer()->Add(m_filepath_page);
}

void
SetupWizard::run()
{
	if (RunWizard(m_filepath_page))
	{
		// Then user completed Wizard rather than cancelling.
		configure_default_commodity();
		create_file();
		try
		{
			configure_accounts();
		}
		catch (...)
		{
			delete_file();
			throw;
		}
	}
	return;
}

wxSize
SetupWizard::standard_text_box_size()
{
	return wxSize(140, 12);
}

Commodity
SetupWizard::selected_currency() const
{
	return m_filepath_page->selected_currency();
}

Decimal
SetupWizard::total_opening_balance() const
{
	JEWEL_ASSERT (m_balance_sheet_account_page);
	return m_balance_sheet_account_page->total_amount();
}

void
SetupWizard::set_assumed_currency(Commodity const& p_commodity)
{
	JEWEL_ASSERT (m_balance_sheet_account_page);	
	m_balance_sheet_account_page->set_commodity(p_commodity);
	m_pl_account_page->set_commodity(p_commodity);
	return;
}

void
SetupWizard::selected_augmented_accounts(vector<AugmentedAccount>& out) const
{
	m_balance_sheet_account_page->selected_augmented_accounts(out);
	m_pl_account_page->selected_augmented_accounts(out);
	return;
}

void
SetupWizard::render_account_pages()
{
	// TODO
	// The call to render_account_pages() should cause
	// the AccountPage(s) to be rendered, in light of whatever currency
	// is currently selected on the previous page. This is because
	// the currency that is selected may infuence the degree of
	// precision, and hence the spacing of the "zero dash" on the
	// AccountPage(s).
	JEWEL_ASSERT (m_balance_sheet_account_page);
	JEWEL_ASSERT (m_pl_account_page);
	m_balance_sheet_account_page->render();
	m_pl_account_page->render();
	return;
}

void
SetupWizard::configure_default_commodity()
{
	Commodity commodity = selected_currency();
	commodity.set_multiplier_to_base(Decimal(1, 0));
	m_database_connection.set_default_commodity(commodity);
	return;
}

void
SetupWizard::create_file()
{
	JEWEL_ASSERT (m_filepath_page);
	JEWEL_ASSERT (m_filepath_page->selected_filepath());
	m_database_connection.open(value(m_filepath_page->selected_filepath()));
	return;
}

void
SetupWizard::delete_file()
{
	JEWEL_ASSERT (m_filepath_page);
	boost::filesystem::remove(value(m_filepath_page->selected_filepath()));
	return;
}

void
SetupWizard::configure_accounts()
{
	vector<AugmentedAccount> augmented_accounts;
	selected_augmented_accounts(augmented_accounts);
	vector<AugmentedAccount>::iterator it = augmented_accounts.begin();
	vector<AugmentedAccount>::iterator const end = augmented_accounts.end();
	DatabaseTransaction transaction(m_database_connection);
	for ( ; it != end; ++it)
	{
		wxString const name_wx = it->account.name().Trim();
		if (name_wx.IsEmpty())
		{
			// TODO React accordingly...
		}
		else
		{
			it->account.set_commodity(selected_currency());
			it->account.set_description(wxString(""));
			it->account.set_visibility(visibility::visible);
			it->account.save();
			JEWEL_ASSERT 
			(	it->technical_opening_balance.places() ==
				selected_currency().precision()
			);
			OrdinaryJournal opening_balance_journal
			(	OrdinaryJournal::create_opening_balance_journal
				(	it->account,
					it->technical_opening_balance
				)
			);
			opening_balance_journal.save();
		}
	}
	transaction.commit();
	return;
}


/*** SetupWizard::FilepathValidator ***/


SetupWizard::FilepathValidator::FilepathValidator
(	filesystem::path* p_filepath
):
	m_filepath(p_filepath)
{
}

SetupWizard::FilepathValidator::FilepathValidator
(	FilepathValidator const& rhs
):
	wxValidator(),
	m_filepath(rhs.m_filepath)
{
}

bool
SetupWizard::FilepathValidator::Validate(wxWindow* WXUNUSED(parent))
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl const* const text_ctrl =
		dynamic_cast<wxTextCtrl*>(GetWindow());	
	if (!text_ctrl)
	{
		return false;
	}
	wxString const wx_filename = with_extension(text_ctrl->GetValue());
	FilepathPage* page =
		dynamic_cast<FilepathPage*>(text_ctrl->GetParent());
	JEWEL_ASSERT (page);
	if (!page)
	{
		return false;
	}
	JEWEL_ASSERT (page->m_directory_ctrl);
	wxString const wx_directory	= page->m_directory_ctrl->GetValue();
	filesystem::path const path =
		wx_to_boost_filepath(wx_directory, wx_filename);
	string filename_error_message;
	bool const filename_is_valid = is_valid_filename
	(	wx_to_std8(wx_filename),
		filename_error_message,
		false  // We don't want explicit extension
	);
	bool const directory_exists =
		filesystem::exists(filesystem::status(path.parent_path()));
	bool const filepath_already_exists = filesystem::exists(path);
	bool const ret =
		filename_is_valid &&
		directory_exists &&
		!filepath_already_exists;
	if (!ret)
	{
		if (!filename_is_valid)
		{
			wxString const message = std8_to_wx(filename_error_message);
			JEWEL_ASSERT (!message.IsEmpty());
			wxMessageBox(message);
		}
		if (!directory_exists)
		{
			wxMessageBox("Folder does not exist.");
		}
		if (filepath_already_exists)
		{
			wxMessageBox
			(	wxString("File named ") +
				wx_filename +
				wxString(" already exists in ") +
				wx_directory +
				wxString(".")
			);
		}
		return false;
	}
	if (m_filepath) *m_filepath = path;
	return ret;
}

bool
SetupWizard::FilepathValidator::TransferFromWindow()
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	if (m_filepath)
	{
		FilepathPage const* const page =
			dynamic_cast<FilepathPage*>(GetWindow()->GetParent());
		if (!page)
		{
			return false;
		}
		optional<filesystem::path> const path =
			page->selected_filepath();
		if (path) *m_filepath = value(path);
	}
	return true;
}

bool
SetupWizard::FilepathValidator::TransferToWindow()
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	if (m_filepath)
	{
		wxTextCtrl* const text_ctrl =
			dynamic_cast<wxTextCtrl*>(GetWindow());
		if (!text_ctrl)
		{
			return false;
		}
		text_ctrl->SetValue
		(	without_extension
			(	std8_to_wx(m_filepath->filename().string())
			)
		);
	}
	return true;
}

wxObject*
SetupWizard::FilepathValidator::Clone() const
{
	return new FilepathValidator(*this);
}


/*** SetupWizard::FilepathPage ***/

BEGIN_EVENT_TABLE(SetupWizard::FilepathPage, wxWizardPageSimple)
	EVT_BUTTON
	(	s_directory_button_id,
		SetupWizard::FilepathPage::on_directory_button_click
	)
	EVT_WIZARD_PAGE_CHANGING
	(	wxID_ANY,
		SetupWizard::FilepathPage::on_wizard_page_changing
	)
END_EVENT_TABLE()

SetupWizard::FilepathPage::FilepathPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_currencies(make_currencies(p_database_connection)),
	m_top_sizer(0),
	m_filename_row_sizer(0),
	m_directory_row_sizer(0),
	m_directory_ctrl(0),
	m_directory_button(0),
	m_filename_ctrl(0),
	m_selected_filepath(0)
{
	m_top_sizer = new wxBoxSizer(wxVERTICAL);
	m_filename_row_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_directory_row_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxSize const dlg_unit_size = SetupWizard::standard_text_box_size();

	// First row
	wxStaticText* directory_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Folder where new file should be saved:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(directory_prompt);

	// Second row
	m_top_sizer->Add(m_directory_row_sizer);
	wxString default_directory = wxEmptyString;
	optional<filesystem::path> const maybe_directory =
		Application::default_directory();
	if (maybe_directory)
	{
		default_directory = std8_to_wx(value(maybe_directory).string());
		JEWEL_ASSERT (!m_selected_filepath);
		m_selected_filepath = new filesystem::path(value(maybe_directory));
	}
	// TODO We should make this a static text field or something that
	// is obviously noneditable so the user doesn't feel frustrated when
	// they try to edit it.
	m_directory_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		default_directory,
		wxDefaultPosition,
		wxDLG_UNIT(this, dlg_unit_size),
		wxTE_READONLY,  // style
		wxDefaultValidator  // TODO Not user-editable anyway
	);
	m_directory_button = new wxButton
	(	this,
		s_directory_button_id,
		wxString("&Browse..."),
		wxDefaultPosition,
		wxSize(wxDefaultSize.x, m_directory_ctrl->GetSize().y)
	);
	m_directory_row_sizer->Add(m_directory_ctrl, wxSizerFlags(1).Expand());
	m_directory_row_sizer->Add(m_directory_button, 0, wxLEFT, 5);

	// Third row
	m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);

	// Fourth row
	wxStaticText* filename_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Name for new file:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(filename_prompt);

	// Fifth row
	m_top_sizer->Add(m_filename_row_sizer);
	wxString const ext = Application::filename_extension();
	m_filename_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		wxString("MyBudget"),
		wxDefaultPosition,
		wxDLG_UNIT(this, dlg_unit_size),
		0,  // style
		FilepathValidator(m_selected_filepath)
	);
	m_filename_row_sizer->Add(m_filename_ctrl, wxSizerFlags(1).Expand());
	wxStaticText* extension_text = new wxStaticText
	(	this,
		wxID_ANY,
		ext,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_BOTTOM
	);
	m_filename_row_sizer->Add(extension_text);

	// Sixth row
	m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);

	// Seventh row
	wxStaticText* currency_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Select your currency"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(currency_prompt);

	// Eighth row
	m_currency_box = new wxComboBox
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, dlg_unit_size),
		0,
		0,
		wxCB_DROPDOWN | wxCB_SORT | wxCB_READONLY
	);
	for (vector<Commodity>::size_type i = 0; i != m_currencies.size(); ++i)
	{
		Commodity const& commodity = m_currencies[i];
		unsigned int const item_index = m_currency_box->Append
		(	commodity.name() +
			wxString(" (") +
			commodity.abbreviation() +
			wxString(")")
		);
		ClientData<Commodity>* const data =
			new ClientData<Commodity>(commodity);

		// Note control will take care of memory management
		// Note also that the association of each item with a particular
		// Commodity stays constant even while the order of items is
		// changed due to sorting - which is what we want.
		m_currency_box->SetClientObject(item_index, data);
	}
	m_currency_box->SetSelection(0);
	m_top_sizer->Add(m_currency_box);

	// Additional space at bottom
	for (size_t i = 0; i != 5; ++i)
	{
		m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);
	}

	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	Layout();
}

SetupWizard::FilepathPage::~FilepathPage()
{
}

optional<filesystem::path>
SetupWizard::FilepathPage::selected_filepath() const
{
	optional<filesystem::path> ret;
	if (m_selected_filepath)
	{
		ret = *m_selected_filepath;
	}
	return ret;
}

Commodity
SetupWizard::FilepathPage::selected_currency() const
{
	unsigned int const index = m_currency_box->GetSelection();
	typedef ClientData<Commodity> Data;
	Data* data = dynamic_cast<Data*>(m_currency_box->GetClientObject(index));
	JEWEL_ASSERT (data != 0);
	return data->data();
}

void
SetupWizard::FilepathPage::on_directory_button_click(wxCommandEvent& event)
{
	wxString default_directory = m_directory_ctrl->GetValue();
	filesystem::path const default_path(wx_to_std8(default_directory));
	if (!filesystem::exists(filesystem::status(default_path)))
	{
		default_directory = wxEmptyString;
	}
	else
	{
		JEWEL_ASSERT (filesystem::absolute(default_path) == default_path);
	}
	wxDirDialog directory_dialog
	(	this,
		wxEmptyString,
		default_directory,
		wxDD_NEW_DIR_BUTTON
	);
	if (directory_dialog.ShowModal() == wxID_OK)
	{
		wxString const wx_directory = directory_dialog.GetPath();
		m_directory_ctrl->ChangeValue(wx_directory);
		*m_selected_filepath = wx_to_boost_filepath
		(	wx_directory,
			with_extension(m_filename_ctrl->GetValue())
		);
	}
	(void)event;  // Silence compiler warning about unused parameter.
	return;
}

void
SetupWizard::FilepathPage::on_wizard_page_changing(wxWizardEvent& event)
{
	(void)event;  // Silence compiler warning about unused parameter
	SetupWizard* const parent = dynamic_cast<SetupWizard*>(GetParent());
	JEWEL_ASSERT (parent);
	parent->set_assumed_currency(selected_currency());
	return;
}



/*** SetupWizard::AccountPage ***/

BEGIN_EVENT_TABLE(SetupWizard::AccountPage, wxWizardPageSimple)
	EVT_BUTTON
	(	s_pop_row_button_id,
		SetupWizard::AccountPage::on_pop_row_button_click
	)
	EVT_BUTTON
	(	s_push_row_button_id,
		SetupWizard::AccountPage::on_push_row_button_click
	)
	EVT_WIZARD_PAGE_CHANGING
	(	wxID_ANY,
		SetupWizard::AccountPage::on_wizard_page_changing
	)
	EVT_WIZARD_PAGE_CHANGED
	(	wxID_ANY,
		SetupWizard::AccountPage::on_wizard_page_changed
	)
END_EVENT_TABLE()

SetupWizard::AccountPage::AccountPage
(	SetupWizard* p_parent,
	account_super_type::AccountSuperType p_account_super_type,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(p_parent),
	m_account_super_type(p_account_super_type),
	m_min_num_accounts(1),
	m_current_row(0),
	m_database_connection(p_database_connection),
	m_top_sizer(0),
	m_multi_account_panel(0),
	m_parent(*p_parent)
{
}

SetupWizard::AccountPage::~AccountPage()
{
}

void
SetupWizard::AccountPage::render()
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);
	increment_row();
	render_main_text();
	render_buttons();
	increment_row();
	increment_row();
	render_account_view();
	increment_row();
	m_top_sizer->Fit(this);
	Fit();
	Layout();
}

void
SetupWizard::AccountPage::selected_augmented_accounts
(	vector<AugmentedAccount>& out
) const
{
	m_multi_account_panel->selected_augmented_accounts(out);
	return;
}

void
SetupWizard::AccountPage::set_commodity
(	Commodity const& p_commodity
)
{
	JEWEL_ASSERT (m_multi_account_panel);
	m_multi_account_panel->set_commodity(p_commodity);
	return;
}

Decimal
SetupWizard::AccountPage::total_amount() const
{
	if (m_multi_account_panel)
	{
		return m_multi_account_panel->total_amount();
	}
	JEWEL_ASSERT (!m_multi_account_panel);
	return Decimal(0, 0);
}

Decimal
SetupWizard::AccountPage::total_balance_sheet_amount() const
{
	return parent().total_opening_balance();
}

void
SetupWizard::AccountPage::render_main_text()
{
	int const width = medium_width() * 3 + standard_gap();
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		main_text(),
		wxDefaultPosition,
		wxSize(width, wxDefaultSize.y),
		wxALIGN_LEFT
	);
	text->Wrap(width);
	top_sizer().Add(text, wxGBPosition(current_row(), 0), wxGBSpan(2, 3));
	return;
}

void
SetupWizard::AccountPage::render_buttons()
{
	wxString const concept_name = account_concept_name(m_account_super_type);
	m_pop_row_button = new wxButton
	(	this,
		s_pop_row_button_id,
		wxString("Remove ") + concept_name,
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	top_sizer().Add
	(	m_pop_row_button,
		wxGBPosition(current_row(), 3),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_push_row_button = new wxButton
	(	this,
		s_push_row_button_id,
		wxString("Add ") + concept_name,
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	top_sizer().Add
	(	m_push_row_button,
		wxGBPosition(current_row(), 4),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	return;
}

void
SetupWizard::AccountPage::render_account_view()
{
	// Create the control for displaying Accounts
	wxSize const size =
		wxDLG_UNIT(this, SetupWizard::standard_text_box_size());
	Commodity const commodity = parent().selected_currency();

	// Add dummy column to right to allow room for scrollbar.
	wxStaticText* dummy = new wxStaticText
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(scrollbar_width_allowance(), 1)
	);
	top_sizer().Add(dummy, wxGBPosition(current_row(), 5));

	// Main body of page.
	m_multi_account_panel = new MultiAccountPanel
	(	this,
		wxSize(MultiAccountPanel::required_width(), size.y * 14),
		database_connection(),
		m_account_super_type,
		commodity,
		m_min_num_accounts
	);
	top_sizer().Add
	(	m_multi_account_panel,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 5)
	);

	refresh_pop_row_button_state();

	// Add an empty row at bottom. This is a hack to prevent the scrolled
	// area from dropping off the bottom. Unclear on why this was happening -
	// but this fixes it.
	increment_row();
	wxStaticText* dummy2 = new wxStaticText
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(1, size.y)
	);
	top_sizer().Add(dummy2, wxGBPosition(current_row(), 0));

	return;
}

void
SetupWizard::AccountPage::refresh_pop_row_button_state()
{
	JEWEL_ASSERT (m_multi_account_panel);
	JEWEL_ASSERT (m_multi_account_panel->num_rows() >= m_min_num_accounts);
	JEWEL_ASSERT (m_pop_row_button);
	if (m_multi_account_panel->num_rows() > m_min_num_accounts)
	{
		m_pop_row_button->Enable();
		m_pop_row_button->SetToolTip(0);
	}
	else
	{
		JEWEL_ASSERT
		(	m_multi_account_panel->num_rows() ==
			m_min_num_accounts
		);
		m_pop_row_button->Disable();
		ostringstream oss;
		oss << m_min_num_accounts;
		wxString const min_rows_string = std8_to_wx(oss.str());
		if (m_min_num_accounts != 1)
		{
			account_concept_name(m_account_super_type) += wxString("s");
		}
		m_pop_row_button->SetToolTip
		(	wxString("You will need at least ") +
			min_rows_string +
			wxString(" ") +
			account_concept_name(m_account_super_type) +
			wxString(" to start off with.")
		);
	}
	return;
}

PhatbooksDatabaseConnection&
SetupWizard::AccountPage::database_connection() const
{
	return m_database_connection;
}

SetupWizard const&
SetupWizard::AccountPage::parent() const
{
	return m_parent;
}

wxGridBagSizer&
SetupWizard::AccountPage::top_sizer()
{
	JEWEL_ASSERT (m_top_sizer);
	return *m_top_sizer;
}

int
SetupWizard::AccountPage::current_row() const
{
	return m_current_row;
}

void
SetupWizard::AccountPage::increment_row()
{
	++m_current_row;
	return;
}

bool
SetupWizard::AccountPage::account_names_valid
(	wxString& error_message
) const
{
	JEWEL_ASSERT (m_multi_account_panel);
	return m_multi_account_panel->account_names_valid(error_message);
}

bool
SetupWizard::AccountPage::account_types_valid
(	wxString& error_message
) const
{
	using account_type::AccountType;
	if (m_account_super_type == account_super_type::balance_sheet)
	{
		JEWEL_ASSERT (m_multi_account_panel);
		JEWEL_ASSERT (m_min_num_accounts >= 1);
		JEWEL_ASSERT (m_multi_account_panel->num_rows() >= 1);
		return true;
	}
	JEWEL_ASSERT (m_account_super_type == account_super_type::pl);
	account_type::AccountType const atypes[] =
	{	account_type::revenue,
		account_type::expense
	};
	account_type::AccountType const* it = jewel::begin(atypes);
	account_type::AccountType const* const end = jewel::end(atypes);
	for ( ; it != end; ++it)
	{
		if (!m_multi_account_panel->account_type_is_selected(*it))
		{
			error_message =
				wxString("You need at least one ") +
				account_type_to_string(*it) +
				wxString(" ") +
				account_concept_name(m_account_super_type) +
				wxString(" to start off with.");
			return false;
		}
	}
	return true;
}

void
SetupWizard::AccountPage::on_pop_row_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused variable
	JEWEL_ASSERT (m_multi_account_panel);
	m_multi_account_panel->pop_row();
	refresh_pop_row_button_state();
	return;
}

void
SetupWizard::AccountPage::on_push_row_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused variable
	JEWEL_ASSERT (m_multi_account_panel);
	m_multi_account_panel->push_row();
	refresh_pop_row_button_state();
	return;
}

void
SetupWizard::AccountPage::on_wizard_page_changing
(	wxWizardEvent& event
)
{
	wxString error_message;
	JEWEL_ASSERT (error_message.IsEmpty());
	if
	(	!account_names_valid(error_message) ||
		!account_types_valid(error_message)
	)
	{
		wxMessageBox(error_message);
		event.Veto();
		return;
	}
	JEWEL_ASSERT (account_names_valid(error_message));
	JEWEL_ASSERT (account_types_valid(error_message));
	return;
}

void
SetupWizard::AccountPage::on_wizard_page_changed
(	wxWizardEvent& event
)
{
	(void)event;  // silence compiler re. unused parameter
	JEWEL_ASSERT (m_multi_account_panel);
	m_multi_account_panel->update_summary();
	m_multi_account_panel->SetFocus();
	return;
}

wxString
SetupWizard::AccountPage::main_text() const
{
	wxString ret(" ");
	switch (m_account_super_type)
	{
	case account_super_type::balance_sheet:
		ret += wxString
		(	"Enter your assets (\"what you own\") and liabilities "
			"(\"what you owe\"), along with the current balances of "
			"each. "
		);
		break;
	case account_super_type::pl:
		ret += wxString
		(	"Enter some revenue and expenditure categories, along with "
			"the amount of funds you want to allocate to each of "
			"these \"envelopes\" to start out with."
		);
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	ret += wxString
	(	" \n\nNote, you can always add to or change these later, so it "
		"doesn't matter much if you can't think of them all now."
	);
	return ret;
}

}  // namespace gui
}  // namesapce phatbooks



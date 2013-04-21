#include "setup_wizard.hpp"
#include "account_type.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "currency_manager.hpp"
#include "default_account_generator.hpp"
#include "filename_validation.hpp"
#include "frame.hpp"
#include "icon.xpm"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/gbsizer.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/validate.h>
#include <wx/wizard.h>
#include <cassert>
#include <map>
#include <string>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using std::map;
using std::string;
using std::vector;

// For debugging
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


namespace filesystem = boost::filesystem;

namespace phatbooks
{
namespace gui
{


namespace
{
	wxString const wx_app_name()
	{
		return bstring_to_wx(Application::application_name());
	}

	wxString const wx_extension()
	{
		return bstring_to_wx(Application::filename_extension());
	}

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
		(	std8_to_bstring(path.extension().string()) ==
			Application::filename_extension()
		)
		{
			return s;
		}
		return s + wxString(bstring_to_wx(Application::filename_extension()));
	}

	wxString without_extension(wxString const& s)
	{
		filesystem::path const path(wx_to_std8(s));
		if
		(	std8_to_bstring(path.extension().string()) ==
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

}  // end anonymous namespace



/*** SetupWizard ***/



SetupWizard::SetupWizard
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizard
	(	0,
		wxID_ANY,
		wx_app_name() + wxString(" Setup Wizard"),
		wxBitmap(icon_xpm),  // TODO Put a proper image here
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection),
	m_filepath_page(0),
	m_localization_page(0),
	m_account_page(0)
{
	assert (!m_database_connection.is_valid());
	m_filepath_page = new FilepathPage(this, m_database_connection);
	m_localization_page = new LocalizationPage(this, m_database_connection);
	m_account_page = new AccountPage(this, m_database_connection);
	wxWizardPageSimple::Chain(m_filepath_page, m_localization_page);
	wxWizardPageSimple::Chain(m_localization_page, m_account_page);
	GetPageAreaSizer()->Add(m_filepath_page);
}


void
SetupWizard::run()
{
	if (RunWizard(m_filepath_page))
	{
		// Then user completed Wizard rather than cancelling.

		// Default Commodity already configured by
		// LocalizationPage event.
		// configure_default_commodity();

		create_file();
		configure_accounts();
	}
	return;
}

wxSize
SetupWizard::standard_text_box_size()
{
	return wxSize(140, 12);
}

void
SetupWizard::configure_default_commodity()
{
	assert (m_localization_page);
	Commodity commodity = m_localization_page->selected_currency();
	commodity.set_multiplier_to_base(Decimal(1, 0));
	m_database_connection.set_default_commodity(commodity);
	return;
}

void
SetupWizard::create_file()
{
	assert (m_filepath_page);
	assert (m_filepath_page->selected_filepath());
	m_database_connection.open(value(m_filepath_page->selected_filepath()));
	return;
}

void
SetupWizard::configure_accounts()
{
	// TODO Implement
	// ...
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
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl const* const text_ctrl =
		dynamic_cast<wxTextCtrl*>(GetWindow());	
	if (!text_ctrl)
	{
		return false;
	}
	wxString const wx_filename = with_extension(text_ctrl->GetValue());
	FilepathPage* page =
		dynamic_cast<FilepathPage*>(text_ctrl->GetParent());
	assert (page);
	if (!page)
	{
		return false;
	}
	assert (page->m_directory_ctrl);
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
		// TODO Do we need to display an error message here?
		if (!filename_is_valid)
		{
			wxString const message = std8_to_wx(filename_error_message);
			assert (!message.IsEmpty());
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
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	if (m_filepath)
	{
		FilepathPage const* const page =
			dynamic_cast<FilepathPage*>(GetWindow()->GetParent());
		if (!page)
		{
			JEWEL_DEBUG_LOG_LOCATION;
			JEWEL_DEBUG_LOG << "Huh!" << endl;
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
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	if (m_filepath)
	{
		wxTextCtrl* const text_ctrl =
			dynamic_cast<wxTextCtrl*>(GetWindow());
		if (!text_ctrl)
		{
			JEWEL_DEBUG_LOG_LOCATION;
			JEWEL_DEBUG_LOG << "Huh!" << endl;
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
END_EVENT_TABLE()



SetupWizard::FilepathPage::FilepathPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection),
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
		assert (!m_selected_filepath);
		m_selected_filepath = new filesystem::path(value(maybe_directory));
	}
	m_directory_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		default_directory,
		wxDefaultPosition,
		wxDLG_UNIT(this, dlg_unit_size),
		wxTE_READONLY,  // style
		wxDefaultValidator  // TODO We need a proper validator here
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
	wxString const ext = wx_extension();
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

	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
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
		assert (filesystem::absolute(default_path) == default_path);
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




/*** SetupWizard::LocalizationPage ***/


SetupWizard::LocalizationPage::LocalizationPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection),
	m_currency_manager(0),
	m_currency_map(0),
	m_top_sizer(0)
{
	m_currency_manager = new CurrencyManager(p_database_connection);
	m_currency_map = new map<wxString, BString>;
	m_top_sizer = new wxBoxSizer(wxVERTICAL);
	wxSize const dlg_unit_size = SetupWizard::standard_text_box_size();

	assert (m_currency_map->empty());

	// First row
	wxStaticText* currency_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Select your currency"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(currency_prompt);

	// Second row
	wxArrayString currency_strings;
	for
	(	vector<Commodity>::const_iterator it =
				m_currency_manager->currencies().begin(),
			end = m_currency_manager->currencies().end();
		it != end;
		++it
	)
	{
		wxString const name = bstring_to_wx(it->name());
		wxString const abbreviation = bstring_to_wx(it->abbreviation());
		wxString const currency_text =
			name + wxString(" (") + abbreviation + wxString(")");
		currency_strings.Add(currency_text);
		(*m_currency_map)[currency_text] = wx_to_bstring(abbreviation);
	}
	m_currency_box = new wxComboBox
	(	this,
		wxID_ANY,
		currency_strings[0],
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(dlg_unit_size.x * 1.4, dlg_unit_size.y)),
		currency_strings,
		wxCB_DROPDOWN | wxCB_SORT | wxCB_READONLY
	);
	m_top_sizer->Add(m_currency_box);

	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
}

SetupWizard::LocalizationPage::~LocalizationPage()
{
	delete m_currency_manager;
	m_currency_manager = 0;

	delete m_currency_map;
	m_currency_map = 0;
}

Commodity
SetupWizard::LocalizationPage::selected_currency() const
{
	return
		m_currency_manager->get_currency_with_abbreviation
		(	(*m_currency_map)[m_currency_box->GetValue()]
		);
}


/*** AccountPage ***/

SetupWizard::AccountPage::AccountPage
(	SetupWizard* parent,	
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection),
	m_default_account_generator(0),
	m_top_sizer(0),
	m_account_tree(0)
{
	m_default_account_generator = new DefaultAccountGenerator
	(	m_database_connection
	);
	m_top_sizer = new wxBoxSizer(wxVERTICAL);
	vector<Account>& accounts = m_default_account_generator->accounts();

	// First row
	wxStaticText* account_prompt_1 = new wxStaticText
	(	this,
		wxID_ANY,
		wxString
		(	"Select the accounts and categories you want to start off with."
		),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(account_prompt_1);

	// Second row
	wxStaticText* account_prompt_2 = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Note you can always change these later."),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(account_prompt_2);

	// Third row
	wxSize const standard_dlg_size = SetupWizard::standard_text_box_size();
	wxSize const tree_dlg_size
	(	standard_dlg_size.x * 1.4,
		standard_dlg_size.y * accounts.size() * 1.5
	);
	m_account_tree = new wxTreeCtrl
	(	this,
		wxID_ANY,
		wxDefaultPosition,
		wxDLG_UNIT(this, tree_dlg_size),
		wxTR_HAS_BUTTONS | wxTR_NO_LINES
	);
	wxTreeItemId const root_id = m_account_tree->AddRoot(wxEmptyString);
	wxTreeItemId const asset_id =
		m_account_tree->AppendItem(root_id, wxString("Assets"));
	wxTreeItemId const liability_id =
		m_account_tree->AppendItem(root_id, wxString("Liabilities"));
	wxTreeItemId const revenue_id =
		m_account_tree->AppendItem(root_id, wxString("Revenue categories"));
	wxTreeItemId const expense_id =
		m_account_tree->AppendItem(root_id, wxString("Expense categories"));
	for
	(	vector<Account>::const_iterator it = accounts.begin(),
			end = accounts.end();
		it != end;
		++it
	)
	{
		wxTreeItemId parent_id = asset_id;
		switch (it->account_type())
		{
		case account_type::asset:
			parent_id = asset_id;
			break;
		case account_type::liability:
			parent_id = liability_id;
			break;
		case account_type::revenue:
			parent_id = revenue_id;
			break;
		case account_type::expense:
			parent_id = expense_id;
			break;
		default:
			assert (false);
		}
		m_account_tree->AppendItem(parent_id, bstring_to_wx(it->name()));
	}	
	m_account_tree->ExpandAll();
	m_top_sizer->Add(m_account_tree);

	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
}

SetupWizard::AccountPage::~AccountPage()
{
	delete m_default_account_generator;
	m_default_account_generator = 0;

	delete m_account_tree;
	m_account_tree = 0;
}


}  // namespace gui
}  // namesapce phatbooks



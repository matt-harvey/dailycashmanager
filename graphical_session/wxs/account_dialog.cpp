#include "account_dialog.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "decimal_text_ctrl.hpp"
#include "frame.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/noncopyable.hpp>
#include <sqloxx/database_transaction.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <cassert>

using sqloxx::DatabaseTransaction;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountDialog, wxDialog)
	EVT_BUTTON
	(	wxID_OK,
		AccountDialog::on_ok_button_click
	)
	EVT_BUTTON
	(	wxID_CANCEL,
		AccountDialog::on_cancel_button_click
	)
END_EVENT_TABLE()


namespace
{
	wxString account_super_type_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case account_super_type::balance_sheet:
			return wxString("Account");
		case account_super_type::pl:
			return wxString("Category");
		default:
			assert (false);
		}
		assert (false);
	}

	wxString account_name_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" name ");
	}

	wxString account_type_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" type ");
	}

	wxString account_description_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		(void)p_account_super_type;  // silence compiler re. unused param.
		return wxString("Description ");
	}

	wxString opening_amount_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case account_super_type::balance_sheet:
			return wxString("Opening balance ");
		case account_super_type::pl:
			return wxString("Initial budget allocation ");
		default:
			assert (false);
		}
		assert (false);
	}
	
}  // end anonymous namespace


AccountDialog::AccountDialog
(	Frame* p_parent,
	Account& p_account,
	account_super_type::AccountSuperType p_account_super_type
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_top_sizer(0),
	m_name_ctrl(0),
	m_account_type_ctrl(0),
	m_description_ctrl(0),
	m_opening_amount_ctrl(0),
	m_account(p_account)
{
	if
	(	m_account.has_id() &&
		(super_type(m_account.account_type()) != p_account_super_type)
	)
	{
		throw InvalidAccountTypeException
		(	"AccountType of Account passed to AccountDialog constructor does "
			"not belong to the AccountSuperType passed to the same "
			"constructor."
		);
	}

	m_top_sizer = new wxGridBagSizer;
	SetSizer(m_top_sizer);

	int row = 0;

	// Row 0

	wxStaticText* name_ctrl_label = new wxStaticText
	(	this,
		wxID_ANY,
		account_name_ctrl_label_string(p_account_super_type),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT  // WARNING This doesn't work, due to bug in wxWidgets
	);
	m_top_sizer->Add
	(	name_ctrl_label,
		wxGBPosition(row, 0),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	wxString name_tmp = wxEmptyString;
	if (m_account.has_id())
	{
		name_tmp = bstring_to_wx(p_account.name());
	}
	m_name_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		name_tmp,
		wxDefaultPosition,
		wxSize(450, wxDefaultSize.y)
	);
	m_top_sizer->Add(m_name_ctrl, wxGBPosition(row, 1));

	++row;

	// Row 1

	wxStaticText* account_type_ctrl_label = new wxStaticText
	(	this,
		wxID_ANY,
		account_type_ctrl_label_string(p_account_super_type),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT  // WARNING This doesn't work, due to bug in wxWidgets
	);
	m_top_sizer->Add
	(	account_type_ctrl_label,
		wxGBPosition(row, 0),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	m_account_type_ctrl = new AccountTypeCtrl
	(	this,
		wxID_ANY,
		m_name_ctrl->GetSize(),
		m_account.database_connection(),
		p_account_super_type
	);
	if (m_account.has_id())
	{	
		m_account_type_ctrl->set_account_type(m_account.account_type());
	}
	m_top_sizer->Add(m_account_type_ctrl, wxGBPosition(row, 1));

	++row;

	// Row 2
	
	wxStaticText* description_label = new wxStaticText
	(	this,
		wxID_ANY,
		account_description_label_string(p_account_super_type),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT
	);
	m_top_sizer->Add
	(	description_label,
		wxGBPosition(row, 0),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	wxString description_tmp = wxEmptyString;
	if (m_account.has_id())
	{
		description_tmp = bstring_to_wx(p_account.description());
	}
	m_description_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		description_tmp,
		wxDefaultPosition,
		m_name_ctrl->GetSize()
	);
	m_top_sizer->Add(m_description_ctrl, wxGBPosition(row, 1));

	++row;

	// Row 3
	
	wxStaticText* opening_amount_ctrl_label = new wxStaticText
	(	this,
		wxID_ANY,
		opening_amount_label_string(p_account_super_type),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT
	);
	m_top_sizer->Add
	(	opening_amount_ctrl_label,
		wxGBPosition(row, 0),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	m_opening_amount_ctrl = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		wxSize(m_name_ctrl->GetSize().x / 2.0, wxDefaultSize.y),
		(	m_account.has_id()?
			m_account.commodity().precision():
			m_account.database_connection().default_commodity().precision()
		),
		false
	);
	m_top_sizer->Add(m_opening_amount_ctrl, wxGBPosition(row, 1));

	++row;

	// Row 4

	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(wxDefaultSize.x, m_name_ctrl->GetSize().y)
	);
	m_top_sizer->Add
	(	m_cancel_button,
		wxGBPosition(row, 0),
		wxDefaultSpan,
		wxALIGN_LEFT
	);

	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save"),
		wxDefaultPosition,
		wxSize(wxDefaultSize.x, m_name_ctrl->GetSize().y)
	);
	m_top_sizer->Add
	(	m_ok_button,
		wxGBPosition(row, 1),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);

	m_top_sizer->Fit(this);
	Fit();
	Layout();
}

void
AccountDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	if (update_account_from_dialog(!m_account.has_id()))
	{
		EndModal(wxID_OK);
	}
	return;
}

void
AccountDialog::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	EndModal(wxID_CANCEL);
}

bool
AccountDialog::update_account_from_dialog(bool p_is_new_account)
{
	DatabaseTransaction transaction(m_account.database_connection());

	Account temp(m_account.database_connection());
	BString const prospective_name =
		wx_to_bstring(m_name_ctrl->GetValue().Trim());
	if (Account::exists(temp.database_connection(), prospective_name))
	{
		wxMessageBox
		(	"There is already an account or category with this name."
		);
		return false;
	}
	if (prospective_name.IsEmpty())
	{
		wxMessageBox("Name cannot be blank.");
		return false;
	}
	temp.set_name(m_name_ctrl->GetValue());
	temp.set_account_type(m_account_type_ctrl->account_type());
	temp.set_description(m_description_ctrl->GetValue());
		
	if (p_is_new_account)
	{
		temp.set_commodity
		(	m_account.database_connection().default_commodity()
		);
	}

	temp.save();
	
	OrdinaryJournal objnl = OrdinaryJournal::create_opening_balance_journal
	(	temp,
		m_opening_amount_ctrl->amount()
	);
	objnl.save();

	m_account = temp;
	transaction.commit();

	wxString msg =
		account_super_type_string(super_type(m_account.account_type()));
	msg += wxString(" has been ");
	msg += (p_is_new_account? wxString(" created."): wxString(" updated."));

	return true;
}

}  // namespace gui
}  // namespace phatbooks

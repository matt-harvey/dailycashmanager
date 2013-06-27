#include "account_dialog.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "decimal_text_ctrl.hpp"
#include "frame.hpp"
#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <cassert>

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
			wxString(" name:");
	}

	wxString account_type_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" type:");
	}

	wxString account_description_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		(void)p_account_super_type;  // silence compiler re. unused param.
		return wxString("Description:");
	}

	wxString opening_amount_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case account_super_type::balance_sheet:
			return wxString("Opening balance:");
		case account_super_type::pl:
			return wxString("Initial budget allocation:");
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
	// TODO HIGH This should throw if both m_account has an ID and the
	// AccountSuperType of m_account does not match p_account_super_type.

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
	m_top_sizer->Add(name_ctrl_label, wxGBPosition(row, 0));
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
	m_top_sizer->Add(account_type_ctrl_label, wxGBPosition(row, 0));
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
	m_top_sizer->Add(description_label, wxGBPosition(row, 0));
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
	m_top_sizer->Add(opening_amount_ctrl_label, wxGBPosition(row, 0));
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
	m_top_sizer->Add(m_cancel_button, wxGBPosition(row, 0));

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
	// TODO HIGH PRIORITY Implement
}

void
AccountDialog::on_cancel_button_click(wxCommandEvent& event)
{
	// TODO HIGH PRIORITY Implement
}


}  // namespace gui
}  // namespace phatbooks
// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_dialog.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "budget_panel.hpp"
#include "decimal_text_ctrl.hpp"
#include "ordinary_journal.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_exceptions.hpp"
#include "sizing.hpp"
#include <boost/noncopyable.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <cassert>

using jewel::Decimal;
using jewel::UninitializedOptionalException;
using sqloxx::DatabaseTransaction;

// For debugging
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


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
		return account_concept_name(p_account_super_type, true);
	}

	wxString account_name_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" name");
	}

	wxString account_type_ctrl_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" type");
	}

	wxString account_description_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		(void)p_account_super_type;  // silence compiler re. unused param.
		return wxString("Description");
	}

	wxString opening_amount_label_string
	(	account_super_type::AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case account_super_type::balance_sheet:
			return wxString("Opening balance");
		case account_super_type::pl:
			return wxString("Initial budget allocation");
		default:
			assert (false);
		}
		assert (false);
	}
	
}  // end anonymous namespace


AccountDialog::AccountDialog
(	wxWindow* p_parent,
	Account& p_account,
	account_super_type::AccountSuperType p_account_super_type
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_current_row(0),
	m_top_sizer(0),
	m_name_ctrl(0),
	m_account_type_ctrl(0),
	m_description_ctrl(0),
	m_opening_amount_ctrl(0),
	m_budget_panel(0),
	m_account(p_account)
{
	assert (p_parent);  // precondition
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
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	// Row 0
	
	wxStaticText* name_ctrl_label = new wxStaticText
	(	this,
		wxID_ANY,
		account_name_ctrl_label_string(p_account_super_type),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.GetY()),
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	name_ctrl_label,
		wxGBPosition(m_current_row, 1),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
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
		wxSize(extra_large_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add
	(	m_name_ctrl,
		wxGBPosition(m_current_row, 2),
		wxGBSpan(1, 3)
	);

	++m_current_row;

	// Row 1

	wxStaticText* account_type_ctrl_label = new wxStaticText
	(	this,
		wxID_ANY,
		account_type_ctrl_label_string(p_account_super_type),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.GetY()),
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	account_type_ctrl_label,
		wxGBPosition(m_current_row, 1),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
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
	m_top_sizer->Add
	(	m_account_type_ctrl,
		wxGBPosition(m_current_row, 2),
		wxGBSpan(1, 3)
	);

	++m_current_row;

	// Row 2
	
	wxStaticText* description_label = new wxStaticText
	(	this,
		wxID_ANY,
		account_description_label_string(p_account_super_type),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.GetY()),
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	description_label,
		wxGBPosition(m_current_row, 1),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
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
	m_top_sizer->Add
	(	m_description_ctrl,
		wxGBPosition(m_current_row, 2),
		wxGBSpan(1, 3)
	);

	++m_current_row;

	// Row 3
	
	wxStaticText* opening_amount_ctrl_label = new wxStaticText
	(	this,
		wxID_ANY,
		opening_amount_label_string(p_account_super_type),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.GetY()),
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	opening_amount_ctrl_label,
		wxGBPosition(m_current_row, 1),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_opening_amount_ctrl = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), wxDefaultSize.y),
		m_account.database_connection().default_commodity().precision(),
		false
	);
	if (m_account.has_id())
	{
		m_opening_amount_ctrl->
			set_amount(m_account.friendly_opening_balance());
	}
	m_top_sizer->Add
	(	m_opening_amount_ctrl,
		wxGBPosition(m_current_row, 2),
		wxGBSpan(1, 1)
	);
	
	// Hack to add some space to right.
	wxStaticText* dummy = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_top_sizer->Add(dummy, wxGBPosition(m_current_row, standard_border()));

	configure_budget_panel();
	configure_buttons();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	CentreOnScreen();
	Layout();
}

void
AccountDialog::configure_budget_panel()
{
	++m_current_row;
	assert (!m_budget_panel);
	if (account_super_type() == account_super_type::balance_sheet)
	{
		// There are no "bottom controls" for balance sheet Accounts.
		return;
	}
	if (m_account == m_account.database_connection().balancing_account())
	{
		// Cannot edit budgets for the budget balancing Account.
		return;
	}
	assert (account_super_type() == account_super_type::pl);
	
	// Make sure m_account has an AccountType.
	assert (m_account_type_ctrl);
	try
	{
		account_type::AccountType dummy = m_account.account_type();
		(void)dummy;  // silence compiler re. unused variable
	}
	catch (UninitializedOptionalException&)
	{
		m_account.set_account_type(m_account_type_ctrl->account_type());
	}
	m_budget_panel = new BudgetPanel(this, m_account);
	m_top_sizer->Add
	(	m_budget_panel,
		wxGBPosition(m_current_row, 1),
		wxGBSpan(1, 4)
	);
	++m_current_row;
	return;
}

void
AccountDialog::configure_buttons()
{
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(medium_width(), m_name_ctrl->GetSize().y),
		wxALIGN_RIGHT  // WARNING Doesn't seem to work
	);
	m_top_sizer->Add
	(	m_cancel_button,
		wxGBPosition(m_current_row, 3),
		wxDefaultSpan,
		wxALIGN_RIGHT  // WARNING Doesn't seem to work
	);
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save"),
		wxDefaultPosition,
		wxSize(medium_width(), m_name_ctrl->GetSize().y)
	);
	m_top_sizer->Add
	(	m_ok_button,
		wxGBPosition(m_current_row, 4),
		wxDefaultSpan,
		wxALIGN_LEFT
	);
	return;
}

account_super_type::AccountSuperType
AccountDialog::account_super_type() const
{
	assert (m_account_type_ctrl);
	return super_type(m_account_type_ctrl->account_type());
}

void
AccountDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	if (update_account_from_dialog(!m_account.has_id()))
	{
		if (!m_budget_panel)
		{
			EndModal(wxID_OK);
		}
		else if (m_budget_panel->process_confirmation())
		{
			EndModal(wxID_OK);
		}
	}
	return;
}

void
AccountDialog::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	EndModal(wxID_CANCEL);
	return;
}

bool
AccountDialog::update_account_from_dialog(bool p_is_new_account)
{
	DatabaseTransaction transaction(m_account.database_connection());

	Account temp = m_account;
	BString const prospective_name =
		wx_to_bstring(m_name_ctrl->GetValue().Trim());
	if (Account::exists(temp.database_connection(), prospective_name))
	{
		bool const name_matches =
		(	bstring_to_wx(m_account.name()).Lower() ==
			bstring_to_wx(prospective_name).Lower()
		);
		if (m_account.has_id() && name_matches)
		{
			// Then everything's OK, the user has just kept the original
			// name.
		}
		else
		{
			wxMessageBox
			(	wxString("There is already ") +
				bstring_to_wx(account_concepts_phrase(true)) +
				wxString(" with this name.")
			);
			return false;
		}
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
	
	Decimal opening_amount = m_opening_amount_ctrl->amount();
	if (super_type(temp.account_type()) == account_super_type::pl)
	{
		// TODO Handle small possibility of overflow here.
		opening_amount = -opening_amount;
	}
	OrdinaryJournal objnl = OrdinaryJournal::create_opening_balance_journal
	(	temp,
		opening_amount
	);
	objnl.save();

	m_account = temp;
	transaction.commit();

	// Notify window higher in the hierarchy that they need to update for
	// changed Account and new OridinaryJournal.
	assert (GetParent());
	wxEventType const event_type =
	(	p_is_new_account?
		PHATBOOKS_ACCOUNT_CREATED_EVENT:
		PHATBOOKS_ACCOUNT_EDITED_EVENT
	);
	PersistentObjectEvent::fire
	(	GetParent(),  // can't use "this" here, or event is missed
		event_type,
		m_account
	);
	PersistentObjectEvent::fire
	(	GetParent(),  // can't use "this" here, or event is missed
		PHATBOOKS_JOURNAL_CREATED_EVENT,
		objnl
	);
	return true;
}

}  // namespace gui
}  // namespace phatbooks

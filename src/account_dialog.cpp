/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "gui/account_dialog.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include "gui/account_type_ctrl.hpp"
#include "gui/budget_panel.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/frame.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/sizing.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/exception.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/handle.hpp>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <stdexcept>

using jewel::Decimal;
using jewel::UninitializedOptionalException;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;

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
	(	AccountSuperType p_account_super_type
	)
	{
		return account_concept_name
		(	p_account_super_type,
			AccountPhraseFlags().set(string_flags::capitalize)
		);
	}

	wxString account_name_ctrl_label_string
	(	AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" name");
	}

	wxString account_type_ctrl_label_string
	(	AccountSuperType p_account_super_type
	)
	{
		return
			account_super_type_string(p_account_super_type) +
			wxString(" type");
	}

	wxString account_description_label_string
	(	AccountSuperType p_account_super_type
	)
	{
		(void)p_account_super_type;  // silence compiler re. unused param.
		return wxString("Description");
	}

	wxString opening_amount_label_string
	(	AccountSuperType p_account_super_type
	)
	{
		switch (p_account_super_type)
		{
		case AccountSuperType::balance_sheet:
			return wxString("Initial balance");
		case AccountSuperType::pl:
			return wxString("Initial budget allocation");
		default:
			JEWEL_HARD_ASSERT (false);
		}
		JEWEL_HARD_ASSERT (false);
	}
	
}  // end anonymous namespace

AccountDialog::AccountDialog
(	wxWindow* p_parent,
	Handle<Account> const& p_account,
	AccountSuperType p_account_super_type
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_current_row(0),
	m_top_sizer(nullptr),
	m_name_ctrl(nullptr),
	m_account_type_ctrl(nullptr),
	m_description_ctrl(nullptr),
	m_opening_amount_ctrl(nullptr),
	m_budget_panel(nullptr),
	m_visibility_ctrl(nullptr),
	m_cancel_button(nullptr),
	m_ok_button(nullptr),
	m_account(p_account)
{
	JEWEL_ASSERT (p_parent);  // precondition
	
	PhatbooksDatabaseConnection& dbc = m_account->database_connection();

	if
	(	m_account->has_id() &&
		(super_type(m_account->account_type()) != p_account_super_type)
	)
	{
		JEWEL_THROW
		(	InvalidAccountTypeException,
			"AccountType of Account passed to AccountDialog constructor does "
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
	if (m_account->has_id())
	{
		name_tmp = p_account->name();
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
		dbc,
		p_account_super_type
	);
	if (m_account->has_id())
	{	
		m_account_type_ctrl->set_account_type(m_account->account_type());

		// Things are just simpler if we prevent the user from changing the
		// AccountType of an existing Account. Suppose we allowed the
		// AccountType to be changed. Then the user could make it so that,
		// say, there are no expense Accounts. Then the TransactionCtrl
		// would become such that it is no longer possible to create
		// a Journal of TransactionType::expenditure_transaction. This
		// means we might have to update the TransactionTypeCtrl as well
		// as the AccountCtrls within the TransactionCtrl. We avoid this
		// complexity by making it impossible for the user to change the
		// AccountType, thus...
		m_account_type_ctrl->Disable();
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
	if (m_account->has_id())
	{
		description_tmp = p_account->description();
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
		dbc.default_commodity()->precision(),
		false
	);
	if (m_account->has_id())
	{
		m_opening_amount_ctrl->
			set_amount(m_account->friendly_opening_balance());
	}
	m_top_sizer->Add
	(	m_opening_amount_ctrl,
		wxGBPosition(m_current_row, 2),
		wxGBSpan(1, 1)
	);

	wxStaticText* opening_amount_date_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString(" at " ) +
			date_format_wx(dbc.entity_creation_date()),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.GetY()),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	opening_amount_date_label,
		wxGBPosition(m_current_row, 3),
		wxGBSpan(1, 2),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	
	// Hack to add some space to right.
	wxStaticText* dummy = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_top_sizer->Add(dummy, wxGBPosition(m_current_row, standard_border()));

	configure_budget_panel();

	++m_current_row;

	configure_bottom_row();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	CentreOnScreen();
	Layout();
}

AccountDialog::~AccountDialog()
{
}

AccountType
AccountDialog::selected_account_type() const
{
	JEWEL_ASSERT (m_account_type_ctrl);
	return m_account_type_ctrl->account_type();
}

void
AccountDialog::configure_budget_panel()
{
	JEWEL_ASSERT (!m_budget_panel);
	if (account_super_type() == AccountSuperType::balance_sheet)
	{
		// There are no "bottom controls" for balance sheet Accounts.
		return;
	}
	if (m_account == m_account->database_connection().balancing_account())
	{
		// Cannot edit budgets for the budget balancing Account.
		return;
	}
	JEWEL_ASSERT (account_super_type() == AccountSuperType::pl);
	
	++m_current_row;
	++m_current_row;

	// Make sure m_account has an AccountType.
	JEWEL_ASSERT (m_account_type_ctrl);
	try
	{
		AccountType dummy = m_account->account_type();
		(void)dummy;  // silence compiler re. unused variable
	}
	catch (UninitializedOptionalException&)
	{
		m_account->set_account_type(m_account_type_ctrl->account_type());
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
AccountDialog::configure_bottom_row()
{
	JEWEL_ASSERT (!m_visibility_ctrl);
	m_visibility_ctrl = new wxCheckBox
	(	this,
		wxID_ANY,
		wxString("&Show in list"),
		wxDefaultPosition,
		wxSize(medium_width(), m_name_ctrl->GetSize().y),
		wxALIGN_RIGHT
	);
	Visibility visibility = Visibility::visible;
	if (m_account->has_id())
	{
		visibility = m_account->visibility();
	}
	m_visibility_ctrl->SetValue(visibility == Visibility::visible);
	m_top_sizer->Add
	(	m_visibility_ctrl,
		wxGBPosition(m_current_row, 2),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(medium_width(), m_name_ctrl->GetSize().y),
		wxALIGN_RIGHT
	);
	m_top_sizer->Add
	(	m_cancel_button,
		wxGBPosition(m_current_row, 3),
		wxDefaultSpan,
		wxALIGN_RIGHT
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

AccountSuperType
AccountDialog::account_super_type() const
{
	JEWEL_ASSERT (m_account_type_ctrl);
	return super_type(m_account_type_ctrl->account_type());
}

void
AccountDialog::on_ok_button_click(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler re. unused parameter.
	Handle<OrdinaryJournal> objnl;
	wxString const prospective_name = m_name_ctrl->GetValue().Trim();
	if (Account::exists(m_account->database_connection(), prospective_name))
	{
		bool clashes = true;
		if (m_account->has_id())
		{
			if (m_account->name().Lower() == prospective_name.Lower())
			{
				// Then everything's OK, the user has just kept the original
				// name, or else has changed the case.
				clashes = false;
			}
		}
		if (clashes)
		{
			wxMessageBox
			(	wxString("There is already ") +
				account_concepts_phrase
				(	AccountPhraseFlags().set(string_flags::include_article)
				) +
				wxString(" with this name.")
			);
			JEWEL_ASSERT (m_name_ctrl);
			m_name_ctrl->SetFocus();
			JEWEL_LOG_TRACE();
			return;
		}
	}
	if (prospective_name.IsEmpty())
	{
		wxMessageBox("Name cannot be blank.");
		JEWEL_LOG_TRACE();
		return;
	}
	JEWEL_LOG_TRACE();
	m_account->set_name(prospective_name);
	m_account->set_account_type(m_account_type_ctrl->account_type());
	m_account->set_description(m_description_ctrl->GetValue());
	m_account->set_visibility
	(	m_visibility_ctrl->GetValue()?
		Visibility::visible:
		Visibility::hidden
	);
	if (!m_account->has_id())
	{
		m_account->set_commodity
		(	m_account->database_connection().default_commodity()
		);
	}
	wxEventType const event_type =
	(	m_account->has_id()?
		PHATBOOKS_ACCOUNT_EDITED_EVENT:
		PHATBOOKS_ACCOUNT_CREATED_EVENT
	);
	m_account->save();
	auto const frame = dynamic_cast<Frame*>(wxTheApp->GetTopWindow());
	JEWEL_ASSERT (frame);
	PersistentObjectEvent::fire(frame, event_type, m_account->id());
	JEWEL_LOG_TRACE();
	Decimal opening_amount = m_opening_amount_ctrl->amount();
	if (super_type(m_account->account_type()) == AccountSuperType::pl)
	{
		// TODO MEDIUM PRIORITY Handle very small possibility of overflow
		// here (currently it would just throw an exception and crash).
		opening_amount = -opening_amount;
	}
	objnl = create_opening_balance_journal
	(	m_account,
		opening_amount
	);
	if (objnl->primary_amount() != Decimal(0, 0))
	{
		objnl->save();
		JEWEL_ASSERT (frame);
		PersistentObjectEvent::fire
		(	frame,
			PHATBOOKS_JOURNAL_CREATED_EVENT,
			objnl->id()
		);
	}
	else
	{
		// The user has not changed the opening balance - and objnl can
		// be ignored - do nothing here.
	}
	if (m_budget_panel) m_budget_panel->process_confirmation();
	EndModal(wxID_OK);
	JEWEL_LOG_TRACE();
	return;
}

void
AccountDialog::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	EndModal(wxID_CANCEL);
	return;
}

}  // namespace gui
}  // namespace phatbooks

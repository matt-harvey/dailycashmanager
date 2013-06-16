// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_ctrl.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "date_ctrl.hpp"
#include "decimal_text_ctrl.hpp"
#include "decimal_validator.hpp"
#include "entry.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "ordinary_journal.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "top_panel.hpp"
#include "transaction_type_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/on_windows.hpp>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/radiobox.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <cassert>
#include <iostream>
#include <vector>

using jewel::Decimal;
using std::endl;
using std::vector;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(TransactionCtrl, wxPanel)
	EVT_BUTTON
	(	wxID_OK,
		TransactionCtrl::on_ok_button_click
	)
	EVT_BUTTON
	(	s_recurring_transaction_button_id,
		TransactionCtrl::on_recurring_transaction_button_click
	)
	EVT_BUTTON
	(	wxID_CANCEL,
		TransactionCtrl::on_cancel_button_click
	)
END_EVENT_TABLE()

// WARNING There are bugs in wxWidgets' wxDatePickerCtrl under wxGTK.
// Firstly, tab traversal gets stuck on that control.
// Secondly, if we type a different date and then press "Enter" for OK,
// the date that actually gets picked up as the transaction date always
// seems to be TODAY's date, not the date actually entered. This appears to
// be an unresolved bug in wxWidgets.
// Note adding wxTAB_TRAVERSAL to style does not seem to fix the problem.
// We have used a simple custom class, DateCtrl here instead, to avoid
// these problems. Might later add a button to pop up a wxCalendarCtrl
// if the user wants one.

TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	vector<Account> const& p_balance_sheet_accounts,
	vector<Account> const& p_pl_accounts,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize
	),
	m_top_sizer(0),
	m_transaction_type_ctrl(0),
	m_primary_amount_ctrl(0),
	m_date_ctrl(0),
	m_cancel_button(0),
	m_recurring_transaction_button(0),
	m_ok_button(0),
	m_actual_vs_budget_ctrl(0),
	m_database_connection(p_database_connection)
{
	assert (m_account_name_boxes.empty());
	assert (m_comment_boxes.empty());
	assert (m_amount_boxes.empty());

	size_t row = 0;	

	// We construct m_ok_button first as we want to be able to refer to its
	// size when sizing certain other controls below. But we will not add
	// the OK button to m_top_sizer till later.
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&OK"),
		wxDefaultPosition,
		wxDefaultSize
	);
	wxSize const ok_button_size = m_ok_button->GetSize();

	// Top sizer
	m_top_sizer = new wxGridBagSizer();
	SetSizer(m_top_sizer);

	m_transaction_type_ctrl = new TransactionTypeCtrl
	(	this,
		wxID_ANY,
		wxSize(ok_button_size.x * 2, wxDefaultSize.y)
	);
	m_top_sizer->Add(m_transaction_type_ctrl, wxGBPosition(row, 1));
	m_primary_amount_ctrl = new DecimalTextCtrl
	(	this,
		s_primary_amount_ctrl_id,
		wxSize(ok_button_size.x * 2, wxDefaultSize.y),
		m_database_connection.default_commodity().precision(),
		false
	);
	m_top_sizer->Add(m_primary_amount_ctrl, wxGBPosition(row, 2));
	wxSize const date_ctrl_sz(ok_button_size.x, wxDefaultSize.y);
	m_date_ctrl = new DateCtrl(this, wxID_ANY, date_ctrl_sz);
	m_top_sizer->Add(m_date_ctrl, wxGBPosition(row, 4));

	row += 2;
	
	// We need the names of all Accounts, to help us
	// construct the wxComboboxes from the which the user will choose
	// Accounts.
	AccountReader const all_account_reader(m_database_connection);

	// Rows for entering Entry details
	typedef vector<Account>::size_type Size;
	Size const sz = p_accounts.size();
	for (Size id = s_min_entry_row_id, i = 0 ; i != sz; ++i, ++id, ++row)
	{
		Account const account = p_accounts[i];
		AccountCtrl* account_name_box = new AccountCtrl
		(	this,
			id,
			account,
			wxSize(ok_button_size.x * 2, wxDefaultSize.y),
			all_account_reader.begin(),
			all_account_reader.end(),
			m_database_connection
		);
		wxSize const account_name_box_size = account_name_box->GetSize();
		wxTextCtrl* comment_ctrl = new wxTextCtrl
		(	this,
			id,
			wxEmptyString,
			wxDefaultPosition,
			wxSize(ok_button_size.x * 4.5, account_name_box_size.y),
			wxALIGN_LEFT
		);
		Decimal::places_type const precision =
			account.commodity().precision();
		DecimalTextCtrl* entry_ctrl = new DecimalTextCtrl
		(	this,
			id,
			wxSize(ok_button_size.x * 2, account_name_box_size.y),
			precision,
			false
		);
		int base_flag = wxLEFT;
		if (i == 0) base_flag |= wxTOP;
		m_top_sizer->Add(account_name_box, wxGBPosition(row, 1));
		m_top_sizer->Add(comment_ctrl, wxGBPosition(row, 2), wxGBSpan(1, 2));
		m_top_sizer->Add(entry_ctrl, wxGBPosition(row, 4));

		m_account_name_boxes.push_back(account_name_box);
		m_comment_boxes.push_back(comment_ctrl);
		m_amount_boxes.push_back(entry_ctrl);
	}

	// Button row
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(ok_button_size.x, ok_button_size.y)
	);
	m_top_sizer->Add(m_cancel_button, wxGBPosition(row, 1));
	m_recurring_transaction_button = new wxButton
	(	this,
		s_recurring_transaction_button_id,
		wxString("&Recurring..."),
		wxDefaultPosition,
		wxSize(ok_button_size.x, ok_button_size.y)
	);
	m_top_sizer->Add(m_recurring_transaction_button, wxGBPosition(row, 2));
	m_top_sizer->Add(m_ok_button, wxGBPosition(row, 4));
	m_ok_button->SetDefault();  // Enter key will now trigger "OK" button

	++row;

	// Radio box for selecting actual vs. budget
	wxArrayString radio_box_strings;
	radio_box_strings.Add(wxString("Actual"));
	radio_box_strings.Add(wxString("Budget"));
	m_actual_vs_budget_ctrl = new wxRadioBox
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		radio_box_strings,
		1,
		wxRA_SPECIFY_COLS
	);
	m_top_sizer->Add(m_actual_vs_budget_ctrl, wxGBPosition(row, 1));

	// "Admin"
	// SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
}

void
TransactionCtrl::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	if (Validate() && TransferDataFromWindow())
	{
		if (is_balanced())
		{
			post_journal();
			TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
			assert (panel);
			panel->update();
		}
		else
		{
			wxMessageBox("Transaction does not balance.");
		}
	}
	return;
}

void
TransactionCtrl::on_recurring_transaction_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	return;
}

void
TransactionCtrl::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
	assert (panel);
	panel->update();
}


void
TransactionCtrl::post_journal() const
{
	OrdinaryJournal journal(m_database_connection);
	journal.set_whether_actual
	(	m_actual_vs_budget_ctrl->GetSelection() == 0
	);
	size_t const sz = m_account_name_boxes.size();
	assert (sz == m_comment_boxes.size());
	assert (sz == m_amount_boxes.size());
	for (size_t i = 0; i != sz; ++i)
	{
		Account const account
		(	m_database_connection,
			wx_to_bstring(wxString(m_account_name_boxes[i]->GetValue()))
		);
		Entry entry(m_database_connection);
		entry.set_account(account);
		entry.set_comment
		(	wx_to_bstring(m_comment_boxes[i]->GetValue())
		);
		Decimal amount = wx_to_decimal
		(	wxString(m_amount_boxes[i]->GetValue()),
			locale()
		);
		if (!journal.is_actual()) amount = -amount;
		amount = round(amount, account.commodity().precision());
		entry.set_amount(amount);
		entry.set_whether_reconciled(false);
		journal.push_entry(entry);
	}
	assert (journal.is_balanced());
	journal.set_comment("");

	// Process date
	journal.set_date(m_date_ctrl->date());

	// Save journal
	journal.save();
}

bool
TransactionCtrl::is_balanced() const
{
	Decimal balance(0, 0);
	vector<DecimalTextCtrl*>::size_type i = 0;
	vector<DecimalTextCtrl*>::size_type const sz = m_amount_boxes.size();
	for ( ; i != sz; ++i)
	{
		balance += wx_to_decimal(m_amount_boxes[i]->GetValue(), locale());
	}
	return balance == Decimal(0, 0);
}


}  // namespace gui
}  // namespace phatbooks

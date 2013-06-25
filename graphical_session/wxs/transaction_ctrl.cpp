// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_ctrl.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "date_ctrl.hpp"
#include "decimal_text_ctrl.hpp"
#include "decimal_validator.hpp"
#include "draft_journal.hpp"
#include "draft_journal_naming_ctrl.hpp"
#include "entry.hpp"
#include "entry_ctrl.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "frequency_ctrl.hpp"
#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "top_panel.hpp"
#include "transaction_type_ctrl.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using boost::optional;
using boost::scoped_ptr;
using jewel::Decimal;
using jewel::value;
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
	m_source_entry_ctrl(0),
	m_destination_entry_ctrl(0),
	m_primary_amount_ctrl(0),
	m_frequency_ctrl(0),
	m_date_ctrl(0),
	m_cancel_button(0),
	m_ok_button(0),
	m_database_connection(p_database_connection)
{
	assert (!p_balance_sheet_accounts.empty() || !p_pl_accounts.empty());
	assert (p_balance_sheet_accounts.size() + p_pl_accounts.size() >= 2);
	
	// Figure out the natural TransactionType given the Accounts we have
	// been passed. We will use this to initialize the TransactionTypeCtrl.
	Account account_x(p_database_connection);
	Account account_y(p_database_connection);
	if (p_balance_sheet_accounts.empty())
	{
		assert (p_pl_accounts.size() >= 2);
		account_x = p_pl_accounts[0];
		account_y = p_pl_accounts[1];
	}
	else if (p_pl_accounts.empty())
	{
		assert (p_balance_sheet_accounts.size() >= 2);
		account_x = p_balance_sheet_accounts[0];
		account_y = p_balance_sheet_accounts[1];
	}
	else
	{
		assert (!p_balance_sheet_accounts.empty());
		assert (!p_pl_accounts.empty());
		account_x = p_balance_sheet_accounts[0];
		account_y = p_pl_accounts[0];
	}
	if (account_y.account_type() == account_type::revenue)
	{
		using std::swap;
		swap(account_x, account_y);
	}
	assert (account_x.has_id());
	assert (account_y.has_id());
	transaction_type::TransactionType const initial_transaction_type =
		natural_transaction_type(account_x, account_y);

	size_t row = 0;	

	// Top sizer
	m_top_sizer = new wxGridBagSizer();
	SetSizer(m_top_sizer);

	m_transaction_type_ctrl = new TransactionTypeCtrl
	(	this,
		s_transaction_type_ctrl_id,
		wxSize(160, wxDefaultSize.y),
		m_database_connection
	);
	m_transaction_type_ctrl->set_transaction_type(initial_transaction_type);
	wxSize const text_box_size = m_transaction_type_ctrl->GetSize();
	m_top_sizer->Add(m_transaction_type_ctrl, wxGBPosition(row, 0));

	m_primary_amount_ctrl = new DecimalTextCtrl
	(	this,
		s_primary_amount_ctrl_id,
		text_box_size,
		m_database_connection.default_commodity().precision(),
		false
	);
	m_top_sizer->Add
	(	m_primary_amount_ctrl,
		wxGBPosition(row, 3),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);

	// We need the names of available Accounts, for the given
	// TransactionType, from which the user will choose
	// Accounts, for each side of the transaction.

	assert_transaction_type_validity(initial_transaction_type);

	// Rows for entering Entry details
	typedef vector<Account>::size_type Size;
	vector<Account> accounts;
	accounts.push_back(account_x);
	accounts.push_back(account_y);

	row += 2;

	// WARNING Temp hack
	assert (accounts.size() >= 2);

	vector<Account> source_accounts;
	source_accounts.push_back(accounts[0]);
	vector<Account> destination_accounts;
	destination_accounts.push_back(accounts[1]);

	m_source_entry_ctrl = new EntryCtrl
	(	this,
		source_accounts,
		m_database_connection,
		initial_transaction_type,
		text_box_size,
		true
	);
	m_destination_entry_ctrl = new EntryCtrl
	(	this,
		destination_accounts,
		m_database_connection,
		initial_transaction_type,
		text_box_size,
		false
	);
	m_top_sizer->Add
	(	m_source_entry_ctrl,
		wxGBPosition(row, 0),
		wxGBSpan(1, 4),
		wxEXPAND
	);
	
	row += 2;

	m_top_sizer->Add
	(	m_destination_entry_ctrl,
		wxGBPosition(row, 0),
		wxGBSpan(1, 4),
		wxEXPAND
	);
	
	row += 2;

	// Date and Frequency controls
	m_frequency_ctrl = new FrequencyCtrl
	(	this,
		wxID_ANY,
		wxSize(text_box_size.x * 3, text_box_size.y)
	);
	m_top_sizer->Add(m_frequency_ctrl, wxGBPosition(row, 0), wxGBSpan(1, 3));
	m_date_ctrl = new DateCtrl
	(	this,
		wxID_ANY,
		wxSize(text_box_size.x, text_box_size.y)
	);
	m_top_sizer->Add(m_date_ctrl, wxGBPosition(row, 3));

	row += 2;

	// Cancel/Clear button
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Clear"),
		wxDefaultPosition,
		wxSize(text_box_size.x, text_box_size.y)
	);
	m_top_sizer->Add(m_cancel_button, wxGBPosition(row, 0));

	// Save/OK button
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save"),
		wxDefaultPosition,
		wxSize(text_box_size.x, text_box_size.y)
	);

	m_top_sizer->Add(m_ok_button, wxGBPosition(row, 3));
	m_ok_button->SetDefault();  // Enter key will now trigger "OK" button

	// "Admin"
	// SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	Layout();
}

void
TransactionCtrl::refresh_for_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	m_source_entry_ctrl->
		refresh_for_transaction_type(p_transaction_type);
	m_destination_entry_ctrl->
		refresh_for_transaction_type(p_transaction_type);
	return;
}

Decimal
TransactionCtrl::primary_amount() const
{
	return m_primary_amount_ctrl->amount();
}

void
TransactionCtrl::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	if (Validate() && TransferDataFromWindow())
	{
		if (is_balanced())
		{
			if (post_journal())
			{
				TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
				assert (panel);
				panel->update();
			}
		}
		else
		{
			wxMessageBox("Transaction does not balance.");
		}
	}
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

bool
TransactionCtrl::post_journal()
{
	ProtoJournal journal;
	assert (m_transaction_type_ctrl->transaction_type());
	transaction_type::TransactionType const ttype =
		value(m_transaction_type_ctrl->transaction_type());
	journal.set_whether_actual(transaction_type_is_actual(ttype));

	size_t const num_entry_controls = 2;
	EntryCtrl const* const entry_controls[num_entry_controls] =
	{	m_source_entry_ctrl,
		m_destination_entry_ctrl
	};
	for (size_t i = 0; i != num_entry_controls; ++i)
	{
		vector<Entry> entries = entry_controls[i]->make_entries();
		for (vector<Entry>::size_type j = 0; j != entries.size(); ++j)
		{
			Entry entry = entries[j];
			journal.push_entry(entry);
		}
	}
	journal.set_comment("");

	optional<Frequency> const maybe_frequency = m_frequency_ctrl->frequency();
	if (maybe_frequency)
	{
		DraftJournal dj(m_database_connection);
		dj.mimic(journal);
		Repeater repeater(m_database_connection);
		gregorian::date const next_date = m_date_ctrl->date();
		Frequency const freq = value(maybe_frequency);

		// Ensure valid combination of Frequency and next posting date.
		if (!is_valid_date_for_interval_type(next_date, freq.step_type()))
		{
			if (freq.step_type() == interval_type::months)
			{
				assert (next_date.day() > 28);
				wxMessageBox
				(	"Next date for this recurring transaction must be "
					"the 29th of the month or earlier."
				);
				return false;
			}
			else
			{
				// TODO If interval_type is month_end, use month_end_for_date
				// function to generate and suggest using the last day of the
				// month instead of the entered date.
				assert (freq.step_type() == interval_type::month_ends);
				assert (month_end_for_date(next_date) != next_date);
				wxMessageBox
				(	"Date must be the last day of the month."
				);
				return false;
			}
		}

		assert (is_valid_date_for_interval_type(next_date, freq.step_type()));
		repeater.set_next_date(next_date);
		repeater.set_frequency(freq);

		// TODO HIGH PRIORITY This can stuff up the next date is
		// invalid for the selected Frequency. Make sure this works
		// OK.
		dj.push_repeater(repeater);
	
		// Get a name for the DraftJournal
		DraftJournalNamingCtrl naming_ctrl(this, m_database_connection);
		if (naming_ctrl.ShowModal() == wxID_OK)
		{
			dj.set_name(naming_ctrl.draft_journal_name());
		}
		else
		{
			// TODO What are circumstances in which this point might
			// be reached? Is our error message adequate?
			wxMessageBox("Error naming recurring transaction.");
			return false;
		}

		assert (dj.is_balanced());
		dj.save();
		JEWEL_DEBUG_LOG << "Posted Journal:\n\n" << dj << endl;
		return true;
	}
	else
	{
		assert (!maybe_frequency);
		OrdinaryJournal oj(m_database_connection);
		oj.mimic(journal);
		oj.set_date(m_date_ctrl->date());
		assert (oj.is_balanced());
		oj.save();
		JEWEL_DEBUG_LOG << "Posted journal:\n\n" << oj << endl;
		return true;
	}
	assert (false);
}

bool
TransactionCtrl::is_balanced() const
{
	Decimal const primary_amt = primary_amount();
	size_t const num_entry_controls = 2;
	EntryCtrl const* const entry_controls[num_entry_controls] =
		{ m_source_entry_ctrl, m_destination_entry_ctrl };
	for (size_t i = 0; i != num_entry_controls; ++i)
	{
		if (!entry_controls[i]->is_balanced())
		{
			return false;
		}
		assert (entry_controls[i]->primary_amount() == primary_amt);
	}
	return true;
}


}  // namespace gui
}  // namespace phatbooks

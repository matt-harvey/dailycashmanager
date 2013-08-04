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
#include "draft_journal_naming_dialog.hpp"
#include "entry.hpp"
#include "entry_ctrl.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "frequency.hpp"
#include "frequency_ctrl.hpp"
#include "gridded_scrolled_panel.hpp"
#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "persistent_journal.hpp"
#include "repeater.hpp"
#include "sizing.hpp"
#include "top_panel.hpp"
#include "transaction_type_ctrl.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/array_utilities.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using boost::optional;
using boost::scoped_ptr;
using jewel::num_elements;
using jewel::Decimal;
using jewel::value;
using std::endl;
using std::vector;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(TransactionCtrl, GriddedScrolledPanel)
	EVT_BUTTON
	(	wxID_OK,
		TransactionCtrl::on_ok_button_click
	)
	EVT_BUTTON
	(	wxID_CANCEL,
		TransactionCtrl::on_cancel_button_click
	)
	EVT_BUTTON
	(	s_delete_button_id,
		TransactionCtrl::on_delete_button_click
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
	wxSize const& p_size,
	vector<Account> const& p_balance_sheet_accounts,
	vector<Account> const& p_pl_accounts,
	PhatbooksDatabaseConnection& p_database_connection
):
	GriddedScrolledPanel(p_parent, p_size, p_database_connection),
	m_transaction_type_ctrl(0),
	m_source_entry_ctrl(0),
	m_destination_entry_ctrl(0),
	m_primary_amount_ctrl(0),
	m_frequency_ctrl(0),
	m_date_ctrl(0),
	m_cancel_button(0),
	m_delete_button(0),
	m_ok_button(0),
	m_journal(0)
{
	assert (!p_balance_sheet_accounts.empty() || !p_pl_accounts.empty());
	assert (p_balance_sheet_accounts.size() + p_pl_accounts.size() >= 2);
	
	// Figure out the natural TransactionType given the Accounts we have
	// been passed. We will use this to initialize the TransactionTypeCtrl.
	Account account_x(database_connection());
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
	assert_transaction_type_validity(initial_transaction_type);
	wxSize text_box_size;
	configure_top_controls
	(	initial_transaction_type,
		text_box_size,
		Decimal(0, database_connection().default_commodity().precision()),
		available_transaction_types(database_connection())
	);

	// Rows for entering Entry details
	typedef vector<Account>::size_type Size;
	vector<Account> accounts;
	accounts.push_back(account_x);
	accounts.push_back(account_y);

	// WARNING Temp hack
	assert (accounts.size() >= 2);

	vector<Account> source_accounts;
	source_accounts.push_back(accounts[0]);
	vector<Account> destination_accounts;
	destination_accounts.push_back(accounts[1]);

	m_source_entry_ctrl = new EntryCtrl
	(	this,
		source_accounts,
		database_connection(),
		initial_transaction_type,
		text_box_size,
		true
	);
	m_destination_entry_ctrl = new EntryCtrl
	(	this,
		destination_accounts,
		database_connection(),
		initial_transaction_type,
		text_box_size,
		false
	);
	top_sizer().Add
	(	m_source_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4),
		wxEXPAND
	);

	increment_row();
	increment_row();

	top_sizer().Add
	(	m_destination_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4),
		wxEXPAND
	);
	
	increment_row();
	increment_row();

	// Date and Frequency controls
	m_frequency_ctrl = new FrequencyCtrl
	(	this,
		wxID_ANY,
		wxSize
		(	medium_width() * 3 + standard_gap() * 2,
			text_box_size.y
		),
		database_connection(),
		true,
		true
	);
	top_sizer().
		Add(m_frequency_ctrl, wxGBPosition(current_row(), 0), wxGBSpan(1, 3));
	m_date_ctrl = new DateCtrl
	(	this,
		wxID_ANY,
		text_box_size
	);
	top_sizer().Add(m_date_ctrl, wxGBPosition(current_row(), 3));

	increment_row();
	increment_row();

	// Cancel/Clear button
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Clear"),
		wxDefaultPosition,
		text_box_size
	);
	top_sizer().Add(m_cancel_button, wxGBPosition(current_row(), 0));

	// Note "Delete button" is not used unless we are editing an
	// existing PersistentJournal.

	// Save/OK button
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save"),
		wxDefaultPosition,
		text_box_size
	);

	top_sizer().Add(m_ok_button, wxGBPosition(current_row(), 3));
	m_ok_button->SetDefault();  // Enter key will now trigger "Save" button

	// "Admin"
	top_sizer().Fit(this);
	top_sizer().SetSizeHints(this);
	FitInside();
	Layout();
}

TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	wxSize const& p_size,
	OrdinaryJournal const& p_journal
):
	GriddedScrolledPanel
	(	p_parent,
		p_size,
		p_journal.database_connection()
	),
	m_transaction_type_ctrl(0),
	m_source_entry_ctrl(0),
	m_destination_entry_ctrl(0),
	m_primary_amount_ctrl(0),
	m_frequency_ctrl(0),
	m_date_ctrl(0),
	m_cancel_button(0),
	m_delete_button(0),
	m_ok_button(0),
	m_journal(0)
{
	// TODO Make it so that, given this is an existing Journal we don't allow the user
	// to edit the TransactionType (or else maybe they can only change the
	// TransactionType to transaction_type::generic_transaction).

	// TODO Make it so that, given this is an existing Journal, the user cannot change
	// the Journal from OrdinaryJournal to DraftJournal or vice versa. This would
	// involve restricting the options available in the FrequencyCtrl.

	m_journal = new OrdinaryJournal(p_journal);
	configure_for_journal_editing();
}

TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	wxSize const& p_size,
	DraftJournal const& p_journal
):
	GriddedScrolledPanel
	(	p_parent,
		p_size,
		p_journal.database_connection()
	),
	m_transaction_type_ctrl(0),
	m_source_entry_ctrl(0),
	m_destination_entry_ctrl(0),
	m_primary_amount_ctrl(0),
	m_frequency_ctrl(0),
	m_date_ctrl(0),
	m_cancel_button(0),
	m_ok_button(0),
	m_journal(0)
{
	m_journal = new DraftJournal(p_journal);
	configure_for_journal_editing();
}

TransactionCtrl::~TransactionCtrl()
{
	delete m_journal;
	m_journal = 0;
	// wxWidgets takes care of deleting the other pointer members
}

void
TransactionCtrl::configure_top_controls
(	transaction_type::TransactionType p_transaction_type,
	wxSize& p_text_box_size,
	Decimal const& p_primary_amount,
	vector<transaction_type::TransactionType> const&
		p_available_transaction_types
)
{
	// Add some space to the right to provide room for scrollbar
	add_dummy_column();

	// Leave enough blank space at the top so that the first row
	// of proper widgets lines up with the "bar" at the top of
	// the panels to the left.
	// TODO Tweak this for different platforms, using conditional
	// compilation.
	increment_row();
	increment_row();

	wxStaticText* dummy = new wxStaticText
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(0, 0)
	);
	top_sizer().Add(dummy, wxGBPosition(current_row(), 0));

	increment_row();
	
	m_transaction_type_ctrl = new TransactionTypeCtrl
	(	this,
		s_transaction_type_ctrl_id,
		wxSize(medium_width(), wxDefaultSize.y),
		database_connection(),
		p_available_transaction_types
	);
	m_transaction_type_ctrl->set_transaction_type(p_transaction_type);
	p_text_box_size = m_transaction_type_ctrl->GetSize();
	top_sizer().Add(m_transaction_type_ctrl, wxGBPosition(current_row(), 0));

	m_primary_amount_ctrl = new DecimalTextCtrl
	(	this,
		s_primary_amount_ctrl_id,
		p_text_box_size,
		database_connection().default_commodity().precision(),
		false
	);
	m_primary_amount_ctrl->set_amount(p_primary_amount);
	top_sizer().Add
	(	m_primary_amount_ctrl,
		wxGBPosition(current_row(), 3),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	increment_row();
	increment_row();

	return;
}

void
TransactionCtrl::configure_for_journal_editing()
{
	assert (m_journal);
	transaction_type::TransactionType const initial_transaction_type
		= m_journal->transaction_type();
	assert_transaction_type_validity(initial_transaction_type);
	wxSize text_box_size;
	vector<transaction_type::TransactionType> available_transaction_types;
	available_transaction_types.push_back(initial_transaction_type);
	using transaction_type::envelope_transaction;
	using transaction_type::generic_transaction;
	if
	(	(initial_transaction_type != envelope_transaction) &&
		(initial_transaction_type != generic_transaction)
	)
	{
		available_transaction_types.push_back(generic_transaction);
	}
	configure_top_controls
	(	initial_transaction_type,
		text_box_size,
		m_journal->primary_amount(),
		available_transaction_types
	);

	vector<Entry> const& entries = m_journal->entries();
	vector<Entry>::size_type const fulcrum = m_journal->fulcrum();
	vector<Entry>::size_type i = 0;
	vector<Entry>::size_type const sz = entries.size();
	vector<Entry> source_entries;
	vector<Entry> destination_entries;
	for ( ; i != fulcrum; ++i) source_entries.push_back(entries[i]);
	assert (i == fulcrum);
	for ( ; i != sz; ++i) destination_entries.push_back(entries[i]);

	m_source_entry_ctrl = new EntryCtrl
	(	this,
		source_entries,
		database_connection(),
		initial_transaction_type,
		text_box_size,
		true
	);
	m_destination_entry_ctrl = new EntryCtrl
	(	this,
		destination_entries,
		database_connection(),
		initial_transaction_type,
		text_box_size,
		false
	);

	top_sizer().Add
	(	m_source_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4),
		wxEXPAND
	);

	increment_row();
	increment_row();

	top_sizer().Add
	(	m_destination_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4),
		wxEXPAND
	);
	
	increment_row();
	increment_row();

	// TODO Factor out code duplicated with other constructor.

	OrdinaryJournal* oj = dynamic_cast<OrdinaryJournal*>(m_journal);
	bool const is_ordinary = static_cast<bool>(oj);
	DraftJournal* dj = dynamic_cast<DraftJournal*>(m_journal);
	bool const is_draft = static_cast<bool>(dj);
	assert (!(is_ordinary && is_draft));
	assert (is_ordinary || is_draft);

	// Date and Frequency controls
	m_frequency_ctrl = new FrequencyCtrl
	(	this,
		wxID_ANY,
		wxSize(text_box_size.x * 3 + standard_gap() * 2, text_box_size.y),
		database_connection(),
		is_ordinary,
		is_draft
	);
	top_sizer().
		Add(m_frequency_ctrl, wxGBPosition(current_row(), 0), wxGBSpan(1, 3));
	optional<Frequency> maybe_frequency;
	if (is_draft)
	{
		// TODO From GUI user cannot possibly create a DraftJournal with no
		// Repeaters, or with more than 1 Repeater. This next bit only works
		// if that remains true, AND non-GUI-created DraftJournals are never
		// accessed from the GUI!
		assert (dj->repeaters().size() == 1);
		maybe_frequency = dj->repeaters().at(0).frequency();
	}	
	m_frequency_ctrl->set_frequency(maybe_frequency);

	gregorian::date date;
	if (is_draft)
	{
		// TODO From GUI user cannot possibly create a DraftJournal with no
		// Repeaters, or with more than 1 Repeater. This next bit only works
		// if that remains true, AND non-GUI-created DraftJournals are never
		// accessed from the GUI!
		assert (dj->repeaters().size() == 1);
		date = dj->repeaters().at(0).next_date();
	}
	else
	{
		assert (is_ordinary);
		date = oj->date();
	}
	m_date_ctrl = new DateCtrl
	(	this,
		wxID_ANY,
		wxSize(text_box_size.x, text_box_size.y),
		date
	);
	top_sizer().Add(m_date_ctrl, wxGBPosition(current_row(), 3));

	increment_row();
	increment_row();

	// Cancel/Clear button
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(text_box_size.x, text_box_size.y)
	);
	top_sizer().Add(m_cancel_button, wxGBPosition(current_row(), 0));

	// Delete button
	m_delete_button = new wxButton
	(	this,
		s_delete_button_id,
		wxString("&Delete transaction"),
		wxDefaultPosition,
		wxSize(text_box_size.x, text_box_size.y)
	);
	top_sizer().Add(m_delete_button, wxGBPosition(current_row(), 1));

	// Save/OK button
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save changes"),
		wxDefaultPosition,
		wxSize(text_box_size.x, text_box_size.y)
	);

	top_sizer().Add(m_ok_button, wxGBPosition(current_row(), 3));
	m_ok_button->SetDefault();  // Enter key will now trigger "OK" button

	// "Admin"
	top_sizer().Fit(this);
	top_sizer().SetSizeHints(this);
	FitInside();
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
TransactionCtrl::add_dummy_column()
{
	wxStaticText* dummy = new wxStaticText
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(scrollbar_width_allowance(), 1)
	);
	top_sizer().Add(dummy, wxGBPosition(0, 4));
}

void
TransactionCtrl::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
	assert (panel);
	panel->configure_transaction_ctrl();
	panel->configure_draft_journal_list_ctrl();
	return;
}

void
TransactionCtrl::on_delete_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	assert (m_journal);
	wxMessageDialog confirmation
	(	this,
		wxString("Are you sure you want to delete this entire transaction?"),
		wxEmptyString,
		wxYES_NO
	);
	int const result = confirmation.ShowModal();
	if (result == wxID_YES)
	{
		PersistentJournal::Id const doomed_journal_id = m_journal->id();
		vector<Entry::Id> doomed_entry_ids;
		vector<Entry> const& doomed_entries = m_journal->entries();
		vector<Entry>::const_iterator it = doomed_entries.begin();
		vector<Entry>::const_iterator const end = doomed_entries.end();
		for ( ; it != end; ++it) doomed_entry_ids.push_back(it->id());
		remove_journal();
		assert (!m_journal->has_id());
		TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
		assert (panel);
		OrdinaryJournal* const oj = dynamic_cast<OrdinaryJournal*>(m_journal);
		if (oj)
		{
			panel->update_for_deleted_ordinary_journal(doomed_journal_id);
			panel->update_for_deleted_ordinary_entries(doomed_entry_ids);
		}
		else
		{
			DraftJournal* const dj = dynamic_cast<DraftJournal*>(m_journal);
			assert (dj);
			panel->update_for_deleted_draft_journal(doomed_journal_id);
			panel->update_for_deleted_draft_entries(doomed_entry_ids);
		}
	}
	return;
}

void
TransactionCtrl::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	assert (m_source_entry_ctrl);
	assert (m_destination_entry_ctrl);
	if (Validate() && TransferDataFromWindow())
	{
		if (is_balanced())
		{
			if (m_journal) save_existing_journal();
			else post_journal();
		}
		else
		{
			wxString msg = wxEmptyString;
			msg += wxString("Cannot save unbalanced transaction.\n");
			Decimal const imbalances[] =
			{	m_source_entry_ctrl->total_amount() - primary_amount(),
				m_destination_entry_ctrl->total_amount() - primary_amount()
			};
			for (size_t i = 0; i != num_elements(imbalances); ++i)
			{
				Decimal const& imbalance = imbalances[i];
				if (imbalance != Decimal(0, 0))
				{
					msg += "Total of ";
					msg += 
					(	(i == 0)?
						wxString("source "):
						wxString("destination ")
					);
					msg += "amounts differs from main transaction amount by ";
					msg += finformat_wx_nopad(imbalance, locale(), false);
					msg += ".\n";
				}
			}
			wxMessageBox(msg);
		}
	}
	return;
}

bool
TransactionCtrl::post_journal()
{
	ProtoJournal journal;
	assert (m_transaction_type_ctrl->transaction_type());
	transaction_type::TransactionType const ttype =
		value(m_transaction_type_ctrl->transaction_type());
	journal.set_transaction_type(ttype);

	size_t const num_entry_controls = 2;
	EntryCtrl const* const entry_controls[num_entry_controls] =
	{	m_source_entry_ctrl,
		m_destination_entry_ctrl
	};
	for (size_t i = 0; i != num_entry_controls; ++i)
	{
		vector<Entry> entries = entry_controls[i]->make_entries();
		if (i == 0)
		{
			journal.set_fulcrum(entries.size());
		}
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
		DraftJournal dj(database_connection());
		dj.mimic(journal);
		Repeater repeater(database_connection());
		assert (m_date_ctrl->date());
		gregorian::date const next_date = value(m_date_ctrl->date());
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
		DraftJournalNamingDialog naming_ctrl(0, database_connection());
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
		TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
		assert (panel);
		panel->update_for_new(dj);
		return true;
	}
	else
	{
		assert (!maybe_frequency);
		OrdinaryJournal oj(database_connection());
		oj.mimic(journal);
		assert (m_date_ctrl->date());
		oj.set_date(value(m_date_ctrl->date()));
		assert (oj.is_balanced());
		oj.save();
		JEWEL_DEBUG_LOG << "Posted journal:\n\n" << oj << endl;
		TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
		assert (panel);
		panel->update_for_new(oj);
		return true;
	}
	assert (false);
}

bool
TransactionCtrl::remove_journal()
{
	assert (m_journal);
	m_journal->remove();
	return true;
}

bool
TransactionCtrl::save_existing_journal()
{
	assert (m_journal);
	assert (m_transaction_type_ctrl->transaction_type());
	transaction_type::TransactionType const ttype =
		value(m_transaction_type_ctrl->transaction_type());
	m_journal->set_transaction_type(ttype);

	// Make a vector containing the source Entries
	vector<Entry> source_entries;
	// Bare scope
	{
		// Populate it with the original source Entries
		vector<Entry>::size_type i = 0;
		vector<Entry>::size_type const f = m_journal->fulcrum();
		for ( ; i != f; ++i)
		{
			source_entries.push_back(m_journal->entries()[i]);
		}
	}
	// Make a vector containing the destination Entries
	vector<Entry> destination_entries;
	// Bare scope
	{
		// Populate it with the original destination Entries
		vector<Entry>::size_type i = m_journal->fulcrum();
		vector<Entry>::size_type const sz = m_journal->entries().size();
		for ( ; i != sz; ++i)
		{
			destination_entries.push_back(m_journal->entries()[i]);
		}
	}
	// Via the EntryCtrl, additional Entries might have been inserted
	// the source Entries, the destination Entries, or both.

	vector<Entry> doomed_entries;

	// Update source entries with data from m_source_entry_ctrl
	// Bare scope
	{
		vector<Entry> fresh_source_entries =
			m_source_entry_ctrl->make_entries();
		vector<Entry>::size_type i = 0;
		vector<Entry>::size_type const sz = source_entries.size();
		for ( ; i != sz; ++i)
		{
			if (i < fresh_source_entries.size())
			{
				JEWEL_DEBUG_LOG << "Mimicking source entry." << endl;
				source_entries[i].mimic(fresh_source_entries[i]);
				assert
				(	source_entries[i].is_reconciled() ==
					fresh_source_entries[i].is_reconciled()
				);
			}
			else
			{
				doomed_entries.push_back(source_entries[i]);
			}
		}	
		for ( ; i < fresh_source_entries.size(); ++i)
		{
			assert (i >= source_entries.size());
			source_entries.push_back(fresh_source_entries[i]);
		}
	}
	// Update destination entries with data from m_destination_entry_ctrl
	// Bare scope
	// TODO Factor out code duplicated between here and the previous block.
	{
		vector<Entry> fresh_destination_entries =
			m_destination_entry_ctrl->make_entries();
		vector<Entry>::size_type i = 0;
		vector<Entry>::size_type const sz = destination_entries.size();
		for ( ; i != sz; ++i)
		{
			if (i < fresh_destination_entries.size())
			{
				JEWEL_DEBUG_LOG << "Mimicking destination entry." << endl;
				destination_entries[i].mimic(fresh_destination_entries[i]);
				assert
				(	destination_entries[i].is_reconciled() ==
					fresh_destination_entries[i].is_reconciled()
				);
			}
			else
			{
				doomed_entries.push_back(destination_entries[i]);
			}
		}
		for ( ; i < fresh_destination_entries.size(); ++i)
		{
			assert (i >= destination_entries.size());
			destination_entries.push_back(fresh_destination_entries[i]);
		}
	}
	// Clear the exiting entries from journal, then reinsert all the updated
	// entries, then remove the doomed entries.
	m_journal->clear_entries();
	// Bare scope
	{
		vector<Entry>::size_type i = 0;
		vector<Entry>::size_type sz = source_entries.size();
		for ( ; i != sz; ++i) m_journal->push_entry(source_entries[i]);

		i = 0;
		sz = destination_entries.size();
		for ( ; i != sz; ++i) m_journal->push_entry(destination_entries[i]);

		i = 0;
		sz = doomed_entries.size();
		for ( ; i != sz; ++i) m_journal->remove_entry(doomed_entries[i]);
	}
		
	optional<Frequency> const maybe_frequency = m_frequency_ctrl->frequency();

	if (maybe_frequency)
	{
		DraftJournal* dj = dynamic_cast<DraftJournal*>(m_journal);
		assert (dj);
		assert (m_date_ctrl->date());
		gregorian::date const next_date = value(m_date_ctrl->date());
		Frequency const freq = value(maybe_frequency);
		
		// Ensure valid combination of Frequency and next posting date
		// TODO Factor out duplicated code between this and post_journal().
		if (!is_valid_date_for_interval_type(next_date, freq.step_type()))
		{
			if (freq.step_type() == interval_type::months)
			{
				assert (next_date.day() > 28);
				wxMessageBox
				(	"Next date for this recurring transaction must be "
					"the 28th of the month or earlier."
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
		
		if (dj->repeaters().empty())
		{
			Repeater repeater(database_connection());
			repeater.set_next_date(next_date);
			repeater.set_frequency(freq);
			dj->push_repeater(repeater);
		}
		else
		{
			// WARNING Does this work? It's like I'm circumventing the constness
			// of DraftJournal::repeaters().
			assert (dj->repeaters().size() >= 1);
			Repeater old_repeater = dj->repeaters()[0];
			old_repeater.set_next_date(next_date);
			old_repeater.set_frequency(freq);
			old_repeater.save();
		}
		assert (dj->is_balanced());
		dj->save();
		JEWEL_DEBUG_LOG << "Saved Journal:\n\n" << *dj << endl;
		TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
		assert (panel);
		panel->update_for_amended(*dj);
		return true;
	}
	else
	{
		assert (!maybe_frequency);
		OrdinaryJournal* oj = dynamic_cast<OrdinaryJournal*>(m_journal);
		assert (oj);
		assert (m_date_ctrl->date());
		oj->set_date(value(m_date_ctrl->date()));
	
		// WARNING temp debug
		if (!oj->is_balanced())
		{
			JEWEL_DEBUG_LOG << "Unbalanced Journal:\n\n" << *oj << endl;
		}

		assert (oj->is_balanced());
		oj->save();
		JEWEL_DEBUG_LOG << "Saved Journal:\n\n" << *oj << endl;
		TopPanel* const panel = dynamic_cast<TopPanel*>(GetParent());
		assert (panel);
		panel->update_for_amended(*oj);
		return true;
	}
	assert (false);
}

bool
TransactionCtrl::is_balanced() const
{
	Decimal const primary_amt = primary_amount();
	EntryCtrl const* const entry_controls[] =
		{ m_source_entry_ctrl, m_destination_entry_ctrl };
	for (size_t i = 0; i != num_elements(entry_controls); ++i)
	{
		if (entry_controls[i]->total_amount() != primary_amt)
		{
			return false;
		}
		assert (entry_controls[i]->primary_amount() == primary_amt);
	}
	return true;
}


}  // namespace gui
}  // namespace phatbooks

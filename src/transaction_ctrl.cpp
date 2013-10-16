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


#include "gui/transaction_ctrl.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "finformat.hpp"
#include "frequency.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "persistent_journal.hpp"
#include "proto_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "persistent_journal.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include "transaction_side.hpp"
#include "gui/account_ctrl.hpp"
#include "gui/date_ctrl.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/decimal_validator.hpp"
#include "gui/draft_journal_naming_dialog.hpp"
#include "gui/entry_group_ctrl.hpp"
#include "gui/frequency_ctrl.hpp"
#include "gui/gridded_scrolled_panel.hpp"
#include "gui/frame.hpp"
#include "gui/locale.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/sizing.hpp"
#include "gui/top_panel.hpp"
#include "gui/transaction_type_ctrl.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/wupdlock.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <unordered_set>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::handle_cast;
using sqloxx::Handle;
using sqloxx::Id;
using std::back_inserter;
using std::begin;
using std::copy;
using std::end;
using std::find;
using std::set;
using std::unordered_set;
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

// NOTE There are bugs in wxWidgets' wxDatePickerCtrl under wxGTK.
// Firstly, tab traversal gets stuck on that control.
// Secondly, if we type a different date and then press "Enter" for OK,
// the date that actually gets picked up as the transaction date always
// seems to be TODAY's date, not the date actually entered. This appears to
// be an unresolved bug in wxWidgets.
// Note adding wxTAB_TRAVERSAL to style does not seem to fix the problem.
// We have used a simple custom class, DateCtrl here instead, to avoid
// these problems.
//
// TODO LOW PRIORITY add a button to pop up a wxCalendarCtrl
// if the user wants one.

namespace
{
	bool contains_reconciled_entry(Journal& p_journal)
	{
		for (Handle<Entry> const& entry: p_journal.entries())
		{
			if (entry->has_id() && entry->is_reconciled())
			{
				return true;
			}
		}
		return false;
	}

}  // end anonymous namespace


TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	wxSize const& p_size,
	Handle<OrdinaryJournal> const& p_journal
):
	GriddedScrolledPanel
	(	p_parent,
		p_size,
		p_journal->database_connection()
	),
	m_transaction_type_ctrl(nullptr),
	m_source_entry_ctrl(nullptr),
	m_destination_entry_ctrl(nullptr),
	m_primary_amount_ctrl(nullptr),
	m_frequency_ctrl(nullptr),
	m_date_ctrl(nullptr),
	m_cancel_button(nullptr),
	m_delete_button(nullptr),
	m_ok_button(nullptr),
	m_journal(handle_cast<PersistentJournal>(p_journal))
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_journal);
	JEWEL_ASSERT (p_journal->has_id());
	JEWEL_ASSERT (m_journal->has_id());
	JEWEL_ASSERT (m_journal->id() == p_journal->id());
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	configure_for_editing_persistent_journal();
}

TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	wxSize const& p_size,
	Handle<DraftJournal> const& p_journal
):
	GriddedScrolledPanel
	(	p_parent,
		p_size,
		p_journal->database_connection()
	),
	m_transaction_type_ctrl(nullptr),
	m_source_entry_ctrl(nullptr),
	m_destination_entry_ctrl(nullptr),
	m_primary_amount_ctrl(nullptr),
	m_frequency_ctrl(nullptr),
	m_date_ctrl(nullptr),
	m_cancel_button(nullptr),
	m_delete_button(nullptr),
	m_ok_button(nullptr),
	m_journal(handle_cast<PersistentJournal>(p_journal))
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_journal);
	JEWEL_ASSERT (p_journal->has_id());
	JEWEL_ASSERT (m_journal->has_id());
	JEWEL_ASSERT (m_journal->id() == p_journal->id());
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	configure_for_editing_persistent_journal();
}

TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	wxSize const& p_size,
	ProtoJournal& p_journal,
	PhatbooksDatabaseConnection& p_database_connection
):
	GriddedScrolledPanel
	(	p_parent,
		p_size,
		p_database_connection
	),
	m_transaction_type_ctrl(nullptr),
	m_source_entry_ctrl(nullptr),
	m_destination_entry_ctrl(nullptr),
	m_primary_amount_ctrl(nullptr),
	m_frequency_ctrl(nullptr),
	m_date_ctrl(nullptr),
	m_cancel_button(nullptr),
	m_delete_button(nullptr),
	m_ok_button(nullptr)
{
	JEWEL_LOG_TRACE();
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	configure_for_editing_proto_journal(p_journal);
}

void
TransactionCtrl::configure_top_controls
(	TransactionType p_transaction_type,
	wxSize& p_text_box_size,
	Decimal const& p_primary_amount,
	vector<TransactionType> const& p_available_transaction_types
)
{
	JEWEL_LOG_TRACE();

	// Add some space to the right to provide room for scrollbar
	add_dummy_column();

	// Leave enough blank space at the top so that the first row
	// of proper widgets lines up with the "bar" at the top of
	// the panels to the left.
	// TODO LOW PRIORITY Tweak this for different platforms, using
	// conditional compilation?
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
		database_connection().default_commodity()->precision(),
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
TransactionCtrl::clear_all()
{
	JEWEL_LOG_TRACE();
	DestroyChildren();
	m_transaction_type_ctrl = nullptr;
	m_source_entry_ctrl = nullptr;
	m_destination_entry_ctrl = nullptr;
	m_primary_amount_ctrl = nullptr;
	m_frequency_ctrl = nullptr;
	m_date_ctrl = nullptr;
	m_cancel_button = nullptr;
	m_delete_button = nullptr;
	m_ok_button = nullptr;
	m_journal = Handle<PersistentJournal>();
	set_row(0);
	return;
}

void
TransactionCtrl::configure_for_editing_proto_journal
(	ProtoJournal& p_journal
)
{
	JEWEL_LOG_TRACE();

	wxSize text_box_size;
	configure_top_controls
	(	p_journal.transaction_type(),
		text_box_size,
		Decimal(0, database_connection().default_commodity()->precision()),
		available_transaction_types(database_connection())
	);
	m_source_entry_ctrl = new EntryGroupCtrl
	(	this,
		text_box_size,
		p_journal,
		TransactionSide::source,
		database_connection()
	);
	m_destination_entry_ctrl = new EntryGroupCtrl
	(	this,
		text_box_size,
		p_journal,
		TransactionSide::destination,
		database_connection()
	);
	JEWEL_ASSERT (text_box_size.x == medium_width());
	top_sizer().Add
	(	m_source_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4)
	);
	increment_row();
	increment_row();
	top_sizer().Add
	(	m_destination_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4)
	);
	increment_row();
	increment_row();

	// Date and Frequency controls
	m_frequency_ctrl = new FrequencyCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width() * 3 + standard_gap() * 2, text_box_size.y),
		database_connection(),
		true,
		true
	);
	top_sizer().
		Add(m_frequency_ctrl, wxGBPosition(current_row(), 0), wxGBSpan(1, 3));
	m_date_ctrl = new DateCtrl
	(	this,
		wxID_ANY,
		text_box_size,
		today(),
		false,
		database_connection().entity_creation_date()	
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
	JEWEL_ASSERT (m_delete_button == nullptr);

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

void
TransactionCtrl::configure_for_editing_persistent_journal()
{
	JEWEL_LOG_TRACE();

	wxWindowUpdateLocker const update_locker(this);

	JEWEL_ASSERT (m_journal);
	TransactionType const initial_transaction_type
		= m_journal->transaction_type();
	assert_transaction_type_validity(initial_transaction_type);
	wxSize text_box_size;
	vector<TransactionType> available_transaction_types;
	available_transaction_types.push_back(initial_transaction_type);
	if
	(	(initial_transaction_type != TransactionType::envelope) &&
		(initial_transaction_type != TransactionType::generic)
	)
	{
		available_transaction_types.push_back(TransactionType::generic);
	}
	configure_top_controls
	(	initial_transaction_type,
		text_box_size,
		m_journal->primary_amount(),
		available_transaction_types
	);

	JEWEL_ASSERT (text_box_size.x == medium_width());

	m_source_entry_ctrl = new EntryGroupCtrl
	(	this,
		text_box_size,
		*m_journal,
		TransactionSide::source,
		database_connection()
	);
	m_destination_entry_ctrl = new EntryGroupCtrl
	(	this,
		text_box_size,
		*m_journal,
		TransactionSide::destination,
		database_connection()
	);
	top_sizer().Add
	(	m_source_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4)
	);
	increment_row();
	increment_row();
	top_sizer().Add
	(	m_destination_entry_ctrl,
		wxGBPosition(current_row(), 0),
		wxGBSpan(1, 4)
	);
	increment_row();
	increment_row();

	auto const dj = sqloxx::handle_cast<DraftJournal>(m_journal);
	auto const oj = sqloxx::handle_cast<OrdinaryJournal>(m_journal);
	bool const is_ordinary = static_cast<bool>(oj);
	bool const is_draft = static_cast<bool>(dj);
	JEWEL_ASSERT (!(is_ordinary && is_draft));
	JEWEL_ASSERT (is_ordinary || is_draft);

	// Date and Frequency controls
	m_frequency_ctrl = new FrequencyCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width() * 3 + standard_gap() * 2, text_box_size.y),
		database_connection(),
		is_ordinary,
		is_draft
	);
	top_sizer().
		Add(m_frequency_ctrl, wxGBPosition(current_row(), 0), wxGBSpan(1, 3));
	optional<Frequency> maybe_frequency;
	if (is_draft)
	{
		// TODO MEDIUM PRIORITY From the GUI, user cannot possibly create a
		// DraftJournal with no
		// Repeaters, or with more than 1 Repeater. This next bit only works
		// if that remains true, AND non-GUI-created DraftJournals are never
		// accessed from the GUI! We should guard against future changes
		// breaking these assertions by putting code in Repeater and/or
		// DraftJournal that actually prevents multiple Repeaters being
		// added to a DraftJournal at that business-object level, and
		// comment it to the effect that this is to protect the
		// integrity of the GUI.
		JEWEL_ASSERT (dj);
		JEWEL_ASSERT (!oj);
		JEWEL_ASSERT (dj->repeaters().size() == 1);
		maybe_frequency = dj->repeaters().at(0)->frequency();
	}	
	m_frequency_ctrl->set_frequency(maybe_frequency);

	gregorian::date date;
	if (is_draft)
	{
		// TODO MEDIUM PRIORITY From the GUI, user cannot possibly create a
		// DraftJournal with no
		// Repeaters, or with more than 1 Repeater. This next bit only works
		// if that remains true, AND non-GUI-created DraftJournals are never
		// accessed from the GUI! We should guard against future changes
		// breaking these assertions by putting code in Repeater and/or
		// DraftJournal that actually prevents multiple Repeaters being
		// added to a DraftJournal at that business-object level, and
		// comment it to the effect that this is to protect the
		// integrity of the GUI.
		JEWEL_ASSERT (dj);
		JEWEL_ASSERT (dj->repeaters().size() == 1);
		date = dj->repeaters().at(0)->next_date();
	}
	else
	{
		JEWEL_ASSERT (is_ordinary);
		JEWEL_ASSERT (oj);
		date = oj->date();
	}
	m_date_ctrl = new DateCtrl
	(	this,
		wxID_ANY,
		text_box_size,
		date,
		false,
		database_connection().entity_creation_date()
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
		text_box_size
	);
	top_sizer().Add(m_cancel_button, wxGBPosition(current_row(), 0));

	// Save/OK button
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save changes"),
		wxDefaultPosition,
		text_box_size
	);
	top_sizer().Add(m_ok_button, wxGBPosition(current_row(), 3));
	m_ok_button->SetDefault();  // Enter key will now trigger "OK" button

	increment_row();
	increment_row();

	// Delete button
	m_delete_button = new wxButton
	(	this,
		s_delete_button_id,
		wxString("&Delete transaction"),
		wxDefaultPosition,
		text_box_size
	);
	top_sizer().Add(m_delete_button, wxGBPosition(current_row(), 0));

	increment_row();

	// If there are any reconciled Entries in the Journal, then
	// make it impossible for the user to edit it.
	reflect_reconciliation_statuses();

	// "Admin"
	top_sizer().Fit(this);
	top_sizer().SetSizeHints(this);
	FitInside();
	Layout();
}

void
TransactionCtrl::refresh_for_transaction_type
(	TransactionType p_transaction_type
)
{
	JEWEL_LOG_TRACE();

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
TransactionCtrl::update_for_new(Handle<Account> const& p_saved_object)
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (m_source_entry_ctrl);
	m_source_entry_ctrl->update_for_new(p_saved_object);
	JEWEL_ASSERT (m_destination_entry_ctrl);
	m_destination_entry_ctrl->update_for_new(p_saved_object);
	return;
}

void
TransactionCtrl::update_for_amended(Handle<Account> const& p_saved_object)
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (m_source_entry_ctrl);
	m_source_entry_ctrl->update_for_amended(p_saved_object);
	JEWEL_ASSERT (m_destination_entry_ctrl);
	m_destination_entry_ctrl->update_for_amended(p_saved_object);
	return;
}

void
TransactionCtrl::update_for_reconciliation_status
(	Handle<Entry> const& p_entry
)
{
	JEWEL_LOG_TRACE();

	(void)p_entry;  // silence compiler re. unused parameter
	reflect_reconciliation_statuses();
	return;
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
	JEWEL_LOG_TRACE();

	(void)event;  // Silence compiler re. unused parameter.
	reset();
	return;
}

void
TransactionCtrl::on_delete_button_click(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();

	(void)event;  // Silence compiler re. unused parameter.
	JEWEL_ASSERT (m_journal);
	wxMessageDialog confirmation
	(	this,
		wxString("Are you sure you want to delete this entire transaction?"),
		wxEmptyString,
		wxYES_NO
	);
	int const result = confirmation.ShowModal();
	if (result == wxID_YES)
	{
		if (remove_journal())
		{
			JEWEL_LOG_TRACE();
			reset();
		}
	}
	JEWEL_LOG_TRACE();
	return;
}

void
TransactionCtrl::on_ok_button_click(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();

	// TODO MEDIUM PRIORITY If the user hits enter while in an
	// AccountCtrl containing an invalid Account name, then the "invalid account
	// name" message comes up twice in succession, i.e. they have to click or
	// Enter twice to get rid of it.

	(void)event;  // Silence compiler re. unused parameter.
	JEWEL_ASSERT (m_source_entry_ctrl);
	JEWEL_ASSERT (m_destination_entry_ctrl);
	if (Validate() && TransferDataFromWindow())
	{
		JEWEL_LOG_TRACE();
		if (is_balanced())
		{
			JEWEL_LOG_TRACE();
			bool actioned = false;
			if (m_journal)
			{
				JEWEL_LOG_TRACE();
				actioned = save_existing_journal();
			}
			else
			{
				JEWEL_LOG_TRACE();
				actioned = post_journal();
			}
			if (actioned)
			{
				JEWEL_LOG_TRACE();
				reset();
			}
		}
		else
		{
			JEWEL_LOG_TRACE();
			wxString msg = wxEmptyString;
			msg += wxString("Cannot save unbalanced transaction.");
			Decimal const imbalances[] =
			{	m_source_entry_ctrl->total_amount() - primary_amount(),
				m_destination_entry_ctrl->total_amount() - primary_amount()
			};
			DecimalFormatFlags const flags =
				DecimalFormatFlags().
					clear(string_flags::dash_for_zero).
					set(string_flags::hard_align_right);
			for
			(	size_t i = 0;
				i != static_cast<size_t>(end(imbalances) - begin(imbalances));
				++i
			)
			{
				Decimal const& imbalance = imbalances[i];
				if (imbalance != Decimal(0, 0))
				{
					msg += "\n\nTotal of ";
					msg += 
					(	(i == 0)?
						wxString("source "):
						wxString("destination ")
					);
					msg += "amounts differs from main transaction amount by ";
					msg += finformat_wx(imbalance, locale(), flags);
					msg += ".";
				}
			}
			JEWEL_LOG_TRACE();
			wxMessageBox(msg);
		}
		JEWEL_LOG_TRACE();
	}
	JEWEL_LOG_TRACE();
	return;
}

void
TransactionCtrl::reset()
{
	JEWEL_LOG_TRACE();

	wxWindowUpdateLocker window_update_locker(this);
	TopPanel* const parent = dynamic_cast<TopPanel*>(GetParent());
	JEWEL_ASSERT (parent);
	ProtoJournal proto_journal = parent->make_proto_journal();
	clear_all();
	configure_for_editing_proto_journal(proto_journal);
	parent->Layout();
	return;
}

void
TransactionCtrl::reflect_reconciliation_statuses()
{
	JEWEL_LOG_TRACE();

	EntryGroupCtrl* const entry_controls[] =
	{	m_source_entry_ctrl,
		m_destination_entry_ctrl
	};
	bool contains_reconciled = false;
	for (EntryGroupCtrl* control: entry_controls)
	{
		JEWEL_ASSERT (control);
		if (control->reflect_reconciliation_statuses())
		{
			contains_reconciled = true;
		}
	}
	wxWindow* const general_controls[] =
	{	m_delete_button,
		m_transaction_type_ctrl,
		m_date_ctrl,
		m_primary_amount_ctrl
	};
	for (wxWindow* control: general_controls)
	{
		if (control)
		{
			JEWEL_LOG_TRACE();
			control->Enable(!contains_reconciled);
		}
	}
	JEWEL_LOG_TRACE();
	return;
}

bool
TransactionCtrl::post_journal()
{
	JEWEL_LOG_TRACE();

	ProtoJournal journal;
	JEWEL_ASSERT (m_transaction_type_ctrl->transaction_type());
	TransactionType const ttype =
		value(m_transaction_type_ctrl->transaction_type());
	journal.set_transaction_type(ttype);

	EntryGroupCtrl const* const entry_controls[] =
	{	m_source_entry_ctrl,
		m_destination_entry_ctrl
	};
	for (EntryGroupCtrl const* const control: entry_controls)
	{
		vector<Handle<Entry> > entries = control->make_entries();
		for (Handle<Entry> entry: entries) journal.push_entry(entry);
	}
	journal.set_comment("");

	optional<Frequency> const maybe_frequency = m_frequency_ctrl->frequency();
	if (maybe_frequency)
	{
		Handle<DraftJournal> dj(database_connection());
		dj->mimic(journal);
		JEWEL_ASSERT (m_date_ctrl->date());
		gregorian::date const next_date = value(m_date_ctrl->date());
		Frequency const freq = value(maybe_frequency);

		// Ensure valid combination of Frequency and next posting date.
		if (!is_valid_date_for_interval_type(next_date, freq.step_type()))
		{
			if (freq.step_type() == IntervalType::months)
			{
				JEWEL_ASSERT (next_date.day() > 28);
				wxMessageBox
				(	"Next date for this recurring transaction must be "
					"the 28th of the month or earlier."
				);
				return false;
			}
			else
			{
				// TODO MEDIUM PRIORITY If interval_type is month_end, use
				// month_end_for_date
				// function to generate and suggest using the last day of the
				// month instead of the entered date.
				JEWEL_ASSERT (freq.step_type() == IntervalType::month_ends);
				JEWEL_ASSERT (month_end_for_date(next_date) != next_date);
				wxMessageBox
				(	"Date must be the last day of the month."
				);
				return false;
			}
		}

		JEWEL_ASSERT
		(	is_valid_date_for_interval_type(next_date, freq.step_type())
		);
		Handle<Repeater> const repeater(database_connection());
		repeater->set_next_date(next_date);
		repeater->set_frequency(freq);
		dj->push_repeater(repeater);
	
		// Get a name for the DraftJournal
		DraftJournalNamingDialog naming_ctrl(0, database_connection());
		if (naming_ctrl.ShowModal() == wxID_OK)
		{
			dj->set_name(naming_ctrl.draft_journal_name());
		}
		else
		{
			// TODO MEDIUM PRIORITY What are circumstances in which this point
			// might be reached? Is our error message adequate?
			wxMessageBox("Error naming recurring transaction.");
			return false;
		}

		JEWEL_ASSERT (dj->is_balanced());
		dj->save();

		JEWEL_ASSERT (dj->has_id());
		PersistentObjectEvent::fire
		(	this,
			PHATBOOKS_JOURNAL_CREATED_EVENT,
			dj->id()
		);
		return true;
	}
	else
	{
		JEWEL_ASSERT (!maybe_frequency);
		Handle<OrdinaryJournal> const oj(database_connection());
		oj->mimic(journal);
		JEWEL_ASSERT (m_date_ctrl->date());
		oj->set_date(value(m_date_ctrl->date()));
		JEWEL_ASSERT (oj->is_balanced());
		oj->save();

		JEWEL_ASSERT (oj->has_id());
		PersistentObjectEvent::fire
		(	this,
			PHATBOOKS_JOURNAL_CREATED_EVENT,
			oj->id()
		);
		return true;
	}
	JEWEL_HARD_ASSERT (false);
}

bool
TransactionCtrl::remove_journal()
{
	JEWEL_LOG_TRACE();
	if (!m_journal || !m_journal->has_id())
	{
		// TODO LOW PRIORITY Figure out if this is dead code. If it is,
		// get rid of it and put an appropriate assertion, and document
		// whatever preconditions (in this or other functions) the
		// "deadness" relies on.
#	ifndef NDEBUG
		if (m_journal)
		{
			for (Handle<Entry> const& entry: m_journal->entries())
			{
				JEWEL_ASSERT (!entry->has_id());
			}
		}
#	endif
		return true;
	}
	JEWEL_ASSERT (m_journal);
	bool const is_draft =
		static_cast<bool>(handle_cast<DraftJournal>(m_journal));
	vector<Id> doomed_entry_ids;
	vector<Handle<Entry> > const& doomed_entries = m_journal->entries();
	for (Handle<Entry> const& entry: doomed_entries)
	{
		doomed_entry_ids.push_back(entry->id());
	}
	JEWEL_ASSERT (m_journal->has_id());
	Id const doomed_journal_id = m_journal->id();
	m_journal->remove();
	JEWEL_ASSERT (!m_journal->has_id());
	wxEventType event_type(0);
	if (is_draft)
	{
		PersistentObjectEvent::notify_doomed_draft_entries
		(	this,
			doomed_entry_ids
		);
		event_type = PHATBOOKS_DRAFT_JOURNAL_DELETED_EVENT;
	}
	else
	{
		PersistentObjectEvent::notify_doomed_ordinary_entries
		(	this,
			doomed_entry_ids
		);
		event_type = PHATBOOKS_ORDINARY_JOURNAL_DELETED_EVENT;
	}
	JEWEL_ASSERT (event_type != static_cast<wxEventType>(0));
	PersistentObjectEvent::fire(this, event_type, doomed_journal_id);

	JEWEL_LOG_TRACE();
	return true;
}

bool
TransactionCtrl::save_existing_journal()
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (m_journal);
	JEWEL_ASSERT (m_transaction_type_ctrl->transaction_type());
	TransactionType const ttype =
		value(m_transaction_type_ctrl->transaction_type());
	m_journal->set_transaction_type(ttype);

	// We need to collect the ids of the removed entries so the
	// GUI can be updated for their removal. We start out assuming all
	// the original entries are "doomed". We will erase from the "doomed"
	// IDs as we verify that each Entry is still present in the edited
	// Journal.
	unordered_set<Id> doomed;
	vector<Handle<Entry> > const& old_entries = m_journal->entries();
	for (Handle<Entry> const& entry: old_entries)
	{
		JEWEL_ASSERT (entry->has_id());
		doomed.insert(entry->id());
	}

	JEWEL_LOG_TRACE();

	// Clear the existing Entries from Journal, then reinsert the updated
	// Entries. As each "surviving" Entry is reinserted, we erase its Id from
	// the "doomed" Ids.
	m_journal->clear_entries();
	EntryGroupCtrl const* const entry_controls[] =
	{	m_source_entry_ctrl,
		m_destination_entry_ctrl
	};
	for (EntryGroupCtrl const* const control: entry_controls)
	{
		auto const entries = control->make_entries();
		for (auto entry: entries)
		{
			m_journal->push_entry(entry);
			if (entry->has_id()) doomed.erase(entry->id());
		}
	}

	JEWEL_LOG_TRACE();

	// We now need to put the ids of the removed entries in a vector so the
	// GUI can be updated for their removal.
	vector<Id> const doomed_entry_ids(doomed.begin(), doomed.end());

	optional<Frequency> const maybe_frequency = m_frequency_ctrl->frequency();
	if (maybe_frequency)
	{
		DraftJournal& dj_ref = dynamic_cast<DraftJournal&>(*m_journal);
		JEWEL_ASSERT (m_date_ctrl->date());
		gregorian::date const next_date = value(m_date_ctrl->date());
		Frequency const freq = value(maybe_frequency);
		
		// Ensure valid combination of Frequency and next posting date
		// TODO LOW PRIORITY Factor out duplicated code between this and
		// post_journal().
		if (!is_valid_date_for_interval_type(next_date, freq.step_type()))
		{
			if (freq.step_type() == IntervalType::months)
			{
				JEWEL_ASSERT (next_date.day() > 28);
				wxMessageBox
				(	"Next date for this recurring transaction must be "
					"the 28th of the month or earlier."
				);
				return false;
			}
			else
			{
				// TODO MEDIUM PRIORITY If interval_type is month_end, use
				// month_end_for_date
				// function to generate and suggest using the last day of the
				// month instead of the entered date.
				JEWEL_ASSERT (freq.step_type() == IntervalType::month_ends);
				JEWEL_ASSERT (month_end_for_date(next_date) != next_date);
				wxMessageBox("Date must be the last day of the month.");
				return false;
			}
		}

		JEWEL_ASSERT (is_valid_date_for_interval_type(next_date, freq.step_type()));
		
		if (dj_ref.repeaters().empty())
		{
			Handle<Repeater> const repeater(database_connection());
			repeater->set_next_date(next_date);
			repeater->set_frequency(freq);
			dj_ref.push_repeater(repeater);
		}
		else
		{
			JEWEL_ASSERT (!dj_ref.repeaters().empty());
			Handle<Repeater> const old_repeater = dj_ref.repeaters()[0];
			old_repeater->set_next_date(next_date);
			old_repeater->set_frequency(freq);
			old_repeater->save();
		}
		JEWEL_ASSERT (dj_ref.is_balanced());
		dj_ref.save();
	
		PersistentObjectEvent::notify_doomed_draft_entries
		(	this,
			doomed_entry_ids
		);
		JEWEL_ASSERT (dj_ref.has_id());
		PersistentObjectEvent::fire
		(	this,
			PHATBOOKS_JOURNAL_EDITED_EVENT,
			dj_ref.id()
		);
		JEWEL_LOG_TRACE();
		return true;
	}
	else
	{
		JEWEL_LOG_TRACE();
		JEWEL_ASSERT (!maybe_frequency);
		OrdinaryJournal& oj_ref = dynamic_cast<OrdinaryJournal&>(*m_journal);
		JEWEL_ASSERT (m_date_ctrl->date());
		oj_ref.set_date(value(m_date_ctrl->date()));
	
		JEWEL_ASSERT (oj_ref.is_balanced());
		oj_ref.save();

		PersistentObjectEvent::notify_doomed_ordinary_entries
		(	this,
			doomed_entry_ids
		);
		JEWEL_ASSERT (oj_ref.has_id());
		PersistentObjectEvent::fire
		(	this,
			PHATBOOKS_JOURNAL_EDITED_EVENT,
			oj_ref.id()
		);
		JEWEL_LOG_TRACE();
		return true;
	}
	JEWEL_HARD_ASSERT (false);
}

bool
TransactionCtrl::is_balanced() const
{
	Decimal const primary_amt = primary_amount();
	EntryGroupCtrl const* const entry_controls[] =
		{ m_source_entry_ctrl, m_destination_entry_ctrl };
	for (EntryGroupCtrl const* const control: entry_controls)
	{
		if (control->total_amount() != primary_amt)
		{
			return false;
		}
		JEWEL_ASSERT (control->primary_amount() == primary_amt);
	}
	return true;
}


}  // namespace gui
}  // namespace phatbooks

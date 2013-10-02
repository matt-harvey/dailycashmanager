// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_ctrl.hpp"
#include "account_handle.hpp"
#include "account_ctrl.hpp"
#include "account_type.hpp"
#include "commodity_handle.hpp"
#include "date.hpp"
#include "date_ctrl.hpp"
#include "decimal_text_ctrl.hpp"
#include "decimal_validator.hpp"
#include "draft_journal_impl.hpp"
#include "draft_journal_handle.hpp"
#include "draft_journal_naming_dialog.hpp"
#include "entry_handle.hpp"
#include "entry_group_ctrl.hpp"
#include "entry_handle.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "frequency.hpp"
#include "frequency_ctrl.hpp"
#include "gridded_scrolled_panel.hpp"
#include "journal.hpp"
#include "ordinary_journal_impl.hpp"
#include "ordinary_journal_handle.hpp"
#include "persistent_journal.hpp"
#include "proto_journal.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "persistent_journal.hpp"
#include "persistent_object_event.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include "sizing.hpp"
#include "top_panel.hpp"
#include "transaction_side.hpp"
#include "transaction_type_ctrl.hpp"
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

namespace
{
	bool contains_reconciled_entry(Journal& p_journal)
	{
		for (EntryHandle const& entry: p_journal.entries())
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
	OrdinaryJournalHandle const& p_journal
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
	m_journal(nullptr)
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (p_journal->has_id());
	
	// TODO Make this nicer once facility is provided by sqloxx::Handle to do so.
	Handle<PersistentJournal>* const j = new Handle<PersistentJournal>
	(	Handle<PersistentJournal>::create<PhatbooksDatabaseConnection, OrdinaryJournalImpl>
		(	p_journal->database_connection(),
			p_journal->id()
		)
	);
	m_journal.reset(j);
	configure_for_editing_persistent_journal();
}

TransactionCtrl::TransactionCtrl
(	TopPanel* p_parent,
	wxSize const& p_size,
	DraftJournalHandle const& p_journal
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
	m_journal(nullptr)
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (p_journal->has_id());

	// TODO Make this nicer once facility is provided by sqloxx::Handle to do so.
	Handle<PersistentJournal>* const j = new Handle<PersistentJournal>
	(	Handle<PersistentJournal>::create<PhatbooksDatabaseConnection, DraftJournalImpl>
		(	p_journal->database_connection(),
			p_journal->id()
		)
	);
	m_journal.reset(j);
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
	m_ok_button(nullptr),
	m_journal(nullptr)
{
	JEWEL_LOG_TRACE();

	configure_for_editing_proto_journal(p_journal);
}

void
TransactionCtrl::configure_top_controls
(	TransactionType p_transaction_type,
	wxSize& p_text_box_size,
	Decimal const& p_primary_amount,
	vector<TransactionType> const&
		p_available_transaction_types
)
{
	JEWEL_LOG_TRACE();

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

	m_journal.reset();
	DestroyChildren();
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
		= (*m_journal)->transaction_type();
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
		(*m_journal)->primary_amount(),
		available_transaction_types
	);

	JEWEL_ASSERT (text_box_size.x == medium_width());

	m_source_entry_ctrl = new EntryGroupCtrl
	(	this,
		text_box_size,
		**m_journal,
		TransactionSide::source,
		database_connection()
	);
	m_destination_entry_ctrl = new EntryGroupCtrl
	(	this,
		text_box_size,
		**m_journal,
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

	// TODO Factor out code duplicated with other constructor.

	OrdinaryJournalImpl* oj = dynamic_cast<OrdinaryJournalImpl*>(m_journal->get());
	bool const is_ordinary = static_cast<bool>(oj);
	DraftJournalImpl* dj = dynamic_cast<DraftJournalImpl*>(m_journal->get());
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
		// TODO From GUI user cannot possibly create a DraftJournal with no
		// Repeaters, or with more than 1 Repeater. This next bit only works
		// if that remains true, AND non-GUI-created DraftJournals are never
		// accessed from the GUI!
		JEWEL_ASSERT (dj->repeaters().size() == 1);
		maybe_frequency = dj->repeaters().at(0)->frequency();
	}	
	m_frequency_ctrl->set_frequency(maybe_frequency);

	gregorian::date date;
	if (is_draft)
	{
		// TODO From GUI user cannot possibly create a DraftJournal with no
		// Repeaters, or with more than 1 Repeater. This next bit only works
		// if that remains true, AND non-GUI-created DraftJournals are never
		// accessed from the GUI!
		JEWEL_ASSERT (dj->repeaters().size() == 1);
		date = dj->repeaters().at(0)->next_date();
	}
	else
	{
		JEWEL_ASSERT (is_ordinary);
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
TransactionCtrl::update_for_new(AccountHandle const& p_saved_object)
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (m_source_entry_ctrl);
	m_source_entry_ctrl->update_for_new(p_saved_object);
	JEWEL_ASSERT (m_destination_entry_ctrl);
	m_destination_entry_ctrl->update_for_new(p_saved_object);
	return;
}

void
TransactionCtrl::update_for_amended(AccountHandle const& p_saved_object)
{
	JEWEL_LOG_TRACE();

	JEWEL_ASSERT (m_source_entry_ctrl);
	m_source_entry_ctrl->update_for_amended(p_saved_object);
	JEWEL_ASSERT (m_destination_entry_ctrl);
	m_destination_entry_ctrl->update_for_amended(p_saved_object);
	return;
}

void
TransactionCtrl::update_for_reconciliation_status(EntryHandle const& p_entry)
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
			reset();
		}
	}
	return;
}

void
TransactionCtrl::on_ok_button_click(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();

	(void)event;  // Silence compiler re. unused parameter.
	JEWEL_ASSERT (m_source_entry_ctrl);
	JEWEL_ASSERT (m_destination_entry_ctrl);
	if (Validate() && TransferDataFromWindow())
	{
		if (is_balanced())
		{
			bool actioned = false;
			if (m_journal)
			{
				actioned = save_existing_journal();
			}
			else
			{
				actioned = post_journal();
			}
			if (actioned)
			{
				reset();
			}
		}
		else
		{
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
			wxMessageBox(msg);
		}
	}
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
			control->Enable(!contains_reconciled);
		}
	}
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
		vector<EntryHandle> entries = control->make_entries();
		for (EntryHandle entry: entries) journal.push_entry(entry);
	}
	journal.set_comment("");

	optional<Frequency> const maybe_frequency = m_frequency_ctrl->frequency();
	if (maybe_frequency)
	{
		DraftJournalHandle dj(database_connection());
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
				// TODO If interval_type is month_end, use month_end_for_date
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

		JEWEL_ASSERT (is_valid_date_for_interval_type(next_date, freq.step_type()));
		RepeaterHandle const repeater(database_connection());
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
			// TODO What are circumstances in which this point might
			// be reached? Is our error message adequate?
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
		OrdinaryJournalHandle const oj(database_connection());
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

	if (!m_journal || !(*m_journal)->has_id())
	{
		// WARNING This might be dead code.
#	ifndef NDEBUG
		if (m_journal)
		{
			for (EntryHandle const& entry: (*m_journal)->entries())
			{
				JEWEL_ASSERT (!entry->has_id());
			}
		}
#	endif
		return true;
	}
	JEWEL_ASSERT (m_journal);
	Id const doomed_journal_id = (*m_journal)->id();
	bool const is_draft =
		journal_id_is_draft(database_connection(), doomed_journal_id);
	vector<Id> doomed_entry_ids;
	vector<EntryHandle> const& doomed_entries = (*m_journal)->entries();
	for (EntryHandle const& entry: doomed_entries)
	{
		doomed_entry_ids.push_back(entry->id());
	}
	(*m_journal)->remove();
	JEWEL_ASSERT (!(*m_journal)->has_id());
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
	(*m_journal)->set_transaction_type(ttype);

	// We need to collect the ids of the removed entries so the
	// GUI can be updated for their removal. We start out assuming all
	// the original entries are "doomed". We will erase from the "doomed"
	// IDs as we verify that each Entry is still present in the edited
	// Journal.
	unordered_set<Id> doomed;
	vector<EntryHandle> const& old_entries = (*m_journal)->entries();
	for (EntryHandle const& entry: old_entries)
	{
		JEWEL_ASSERT (entry->has_id());
		doomed.insert(entry->id());
	}

	JEWEL_LOG_TRACE();

	// Clear the existing Entries from Journal, then reinsert the updated
	// Entries. As each "surviving" Entry is reinserted, we erase its Id from
	// the "doomed" Ids.
	(*m_journal)->clear_entries();
	EntryGroupCtrl const* const entry_controls[] =
	{	m_source_entry_ctrl,
		m_destination_entry_ctrl
	};
	for (EntryGroupCtrl const* const control: entry_controls)
	{
		vector<EntryHandle> entries = control->make_entries();
		for (EntryHandle entry: entries)
		{
			(*m_journal)->push_entry(entry);
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
		DraftJournalImpl* dj = dynamic_cast<DraftJournalImpl*>(m_journal->get());
		JEWEL_ASSERT (dj);
		JEWEL_ASSERT (m_date_ctrl->date());
		gregorian::date const next_date = value(m_date_ctrl->date());
		Frequency const freq = value(maybe_frequency);
		
		// Ensure valid combination of Frequency and next posting date
		// TODO Factor out duplicated code between this and post_journal().
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
				// TODO If interval_type is month_end, use month_end_for_date
				// function to generate and suggest using the last day of the
				// month instead of the entered date.
				JEWEL_ASSERT (freq.step_type() == IntervalType::month_ends);
				JEWEL_ASSERT (month_end_for_date(next_date) != next_date);
				wxMessageBox("Date must be the last day of the month.");
				return false;
			}
		}

		JEWEL_ASSERT (is_valid_date_for_interval_type(next_date, freq.step_type()));
		
		if (dj->repeaters().empty())
		{
			RepeaterHandle const repeater(database_connection());
			repeater->set_next_date(next_date);
			repeater->set_frequency(freq);
			dj->push_repeater(repeater);
		}
		else
		{
			JEWEL_ASSERT (!dj->repeaters().empty());
			RepeaterHandle const old_repeater = dj->repeaters()[0];
			old_repeater->set_next_date(next_date);
			old_repeater->set_frequency(freq);
			old_repeater->save();
		}
		JEWEL_ASSERT (dj->is_balanced());
		dj->save();
	
		PersistentObjectEvent::notify_doomed_draft_entries
		(	this,
			doomed_entry_ids
		);
		JEWEL_ASSERT (dj->has_id());
		PersistentObjectEvent::fire
		(	this,
			PHATBOOKS_JOURNAL_EDITED_EVENT,
			dj->id()
		);
		JEWEL_LOG_TRACE();
		return true;
	}
	else
	{
		JEWEL_LOG_TRACE();
		JEWEL_ASSERT (!maybe_frequency);
		OrdinaryJournalImpl* oj = dynamic_cast<OrdinaryJournalImpl*>(m_journal->get());
		JEWEL_ASSERT (oj);
		JEWEL_ASSERT (m_date_ctrl->date());
		oj->set_date(value(m_date_ctrl->date()));
	
		JEWEL_ASSERT (oj->is_balanced());
		JEWEL_LOG_TRACE();
		oj->save();
		JEWEL_LOG_TRACE();

		PersistentObjectEvent::notify_doomed_ordinary_entries
		(	this,
			doomed_entry_ids
		);
		JEWEL_ASSERT (oj->has_id());
		PersistentObjectEvent::fire
		(	this,
			PHATBOOKS_JOURNAL_EDITED_EVENT,
			oj->id()
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

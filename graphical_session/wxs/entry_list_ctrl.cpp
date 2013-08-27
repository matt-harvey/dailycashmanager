// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "entry_list_ctrl.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "app.hpp"
#include "b_string.hpp"
#include "bs_account_entry_list_ctrl.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "entry_reader.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "pl_account_entry_list_ctrl.hpp"
#include "reconciliation_entry_list_ctrl.hpp"
#include "summary_datum.hpp"
#include "unfiltered_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/progdlg.h>
#include <wx/scrolwin.h>
#include <vector>
#include <string>

using boost::lexical_cast;
using boost::optional;
using jewel::value;
using std::string;
using std::vector;

// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(EntryListCtrl, wxListCtrl)
	EVT_LIST_ITEM_ACTIVATED
	(	wxID_ANY,
		EntryListCtrl::on_item_activated
	)
END_EVENT_TABLE()

EntryListCtrl::EntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		p_size,
		wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection)
{
}

EntryListCtrl*
EntryListCtrl::create_actual_ordinary_entry_list
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
)
{
	EntryListCtrl* ret = new UnfilteredEntryListCtrl
	(	p_parent,
		p_size,
		p_database_connection
	);
	initialize(ret);
	return ret;
}

EntryListCtrl*
EntryListCtrl::create_actual_ordinary_entry_list
(	wxWindow* p_parent,
	wxSize const& p_size,
	Account const& p_account,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
)
{
	EntryListCtrl* ret = 0;
	switch (super_type(p_account.account_type()))
	{
	case account_super_type::balance_sheet:
		ret = new BSAccountEntryListCtrl
		(	p_parent,
			p_size,
			p_account,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	case account_super_type::pl:
		ret = new PLAccountEntryListCtrl
		(	p_parent,
			p_size,
			p_account,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	default:
		assert (false);  // Execution never reaches here.
	}
	assert (ret);
	initialize(ret);
	return ret;
}

ReconciliationEntryListCtrl*
EntryListCtrl::create_reconciliation_entry_list
(	ReconciliationListPanel* p_parent,
	wxSize const& p_size,
	Account const& p_account,
	gregorian::date const& p_min_date,
	gregorian::date const& p_max_date
)
{
	assert
	(	super_type(p_account.account_type()) ==
		account_super_type::balance_sheet
	);
	ReconciliationEntryListCtrl* ret = new ReconciliationEntryListCtrl
	(	p_parent,
		p_size,
		p_account,
		p_min_date,
		p_max_date
	);
	initialize(ret);
	return ret;
}

void
EntryListCtrl::initialize(EntryListCtrl* p_entry_list_ctrl)
{
	p_entry_list_ctrl->insert_columns();
	p_entry_list_ctrl->do_initialize_summary_data();
	p_entry_list_ctrl->populate();
	p_entry_list_ctrl->set_column_widths();
	p_entry_list_ctrl->Fit();
	p_entry_list_ctrl->Layout();
	return;
}

void
EntryListCtrl::insert_columns()
{
	insert_date_column();
	do_insert_non_date_columns();
	return;
}

void
EntryListCtrl::insert_date_column()
{
	InsertColumn(date_col_num(), "Date", wxLIST_FORMAT_RIGHT);
	return;
}

void
EntryListCtrl::populate()
{
	boost::scoped_ptr<EntryReader> const reader(do_make_entry_reader());
	EntryReader::const_iterator it = reader->begin();
	EntryReader::const_iterator const end = reader->end();
	if (do_require_progress_log())
	{
		EntryReader::size_type i = 0;
		EntryReader::size_type progress = 0;
		EntryReader::size_type const progress_scaling_factor = 32;
		EntryReader::size_type const progress_max =
			reader->size() / progress_scaling_factor;
		wxProgressDialog progress_dialog
		(	wxEmptyString,
			"Loading transactions...",
			progress_max,
			this,
			wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxRESIZE_BORDER
		);
		for ( ; it != end; ++it, ++i)
		{
			process_push_candidate_entry(*it);
			if (i % progress_scaling_factor == 0)
			{
				assert (progress <= progress_max);
				progress_dialog.Update(progress);
				++progress;
			}
		}
		progress_dialog.Destroy();
	}
	else
	{
		for ( ; it != end; ++it)
		{
			process_push_candidate_entry(*it);
		}
	}
	return;
}

void
EntryListCtrl::autosize_column_widths()
{
	int const num_cols = num_columns();
	for (int i = 0; i != num_cols; ++i) SetColumnWidth(i, wxLIST_AUTOSIZE);
	return;
}

void
EntryListCtrl::adjust_comment_column_to_fit()
{
	int total_widths = 0;
	int const num_cols = num_columns();
	for (int i = 0; i != num_cols; ++i) total_widths += GetColumnWidth(i);
	int const shortfall =
		GetSize().GetWidth() - total_widths - scrollbar_width_allowance();
	int const current_comment_width =
		GetColumnWidth(do_get_comment_col_num());
	SetColumnWidth
	(	do_get_comment_col_num(),
		current_comment_width + shortfall
	);
	return;

}

gregorian::date
EntryListCtrl::date_displayed(long p_row) const
{
	assert (date_col_num() == 0);
	optional<gregorian::date> const maybe_date = parse_date
	(	GetItemText(p_row),
		locale()
	);
	assert (maybe_date);
	return value(maybe_date);
}

long
EntryListCtrl::row_for_date(gregorian::date const& p_date) const
{
	long min = 0;
	long const num_rows = GetItemCount();
	long max = num_rows;

	// Deal with common or special cases
	if (max == 0)
	{
		// No Entries displayed
		return 0;		
	}
	assert (max == num_rows);
	assert (max > 0);
	if (date_displayed(max - 1) <= p_date)
	{
		// Very end
		return max;
	}
	// Date is neither first nor last... find it using binary search.
	while (true)
	{
		assert (min < max);
		long guess = (min + max) / 2;
		if (guess == min)
		{
			// Find end of contiguous rows showing this date
			while ((date_displayed(guess) == p_date) && (guess != num_rows))
			{
				++guess;
			}
			return guess;
		}
		gregorian::date const date = date_displayed(guess);
		assert (guess > 0);
		gregorian::date const predecessor_date = date_displayed(guess - 1);
		assert (predecessor_date <= date);
		if ((predecessor_date <= p_date) && (p_date <= date))
		{
			// Find end of contiguous rows showing this date
			while ((date_displayed(guess) == p_date) && (guess != num_rows))
			{
				++guess;
			}
			return guess;
		}
		assert ((p_date < predecessor_date) || (date < p_date));
		if (p_date < predecessor_date)
		{
			assert (guess < max);
			max = guess;
		}
		else if (date < p_date)
		{
			assert (guess > min);
			min = guess;
		}
	}
}

void
EntryListCtrl::set_column_widths()
{
	do_set_column_widths();
	return;
}

void
EntryListCtrl::process_push_candidate_entry(Entry const& p_entry)
{
	assert (p_entry.has_id());
	do_process_candidate_entry_for_summary(p_entry);
	if (do_approve_entry(p_entry)) push_back_entry(p_entry);
	return;
}

void
EntryListCtrl::process_insertion_candidate_entry
(	Entry const& p_entry,
	long p_row
)
{
	assert (p_entry.has_id());
	do_process_candidate_entry_for_summary(p_entry);
	if (do_approve_entry(p_entry)) insert_entry(p_entry, p_row);
	return;
}

void
EntryListCtrl::do_update_for_amended(Account const& p_account)
{
	(void)p_account;  // Silence compiler re. unused variable
	return;
}

void
EntryListCtrl::on_item_activated(wxListEvent& event)
{
	Entry entry
	(	database_connection(),
		GetItemData(event.GetIndex())
	);
	OrdinaryJournal journal = entry.journal<OrdinaryJournal>();

	// Fire a PersistentJournal editing request. This will be handled
	// higher up the window hierarchy.
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_JOURNAL_EDITING_EVENT,
		journal
	);
	return;
}

EntryListCtrl::~EntryListCtrl()
{
}

void
EntryListCtrl::update_for_new(OrdinaryJournal const& p_journal)
{
	if (p_journal.is_actual())
	{
		vector<Entry>::const_iterator it = p_journal.entries().begin();
		vector<Entry>::const_iterator const end = p_journal.entries().end();
		for ( ; it != end; ++it) process_insertion_candidate_entry(*it);
	}
	set_column_widths();
	return;
}

void
EntryListCtrl::update_for_amended(OrdinaryJournal const& p_journal)
{
	if (!p_journal.is_actual())
	{
		return;
	}
	assert (p_journal.is_actual());
	vector<Entry>::const_iterator it = p_journal.entries().begin();
	vector<Entry>::const_iterator const end = p_journal.entries().end();
	wxString const wx_date_string = date_format_wx(p_journal.date());
	for ( ; it != end; ++it)
	{
		long updated_pos = -1;
		assert (it->has_id());
		IdSet::const_iterator const jt = m_id_set.find(it->id());
		if (jt != m_id_set.end())
		{
			long const pos = FindItem(-1, it->id());
			assert
			(	GetItemData(pos) ==
				static_cast<unsigned long>(it->id())
			);
			gregorian::date const old_date = date_displayed(pos);
			do_process_removal_for_summary(pos);
			DeleteItem(pos);
			m_id_set.erase(jt);
			if (old_date == it->date())
			{
				updated_pos = pos;	
			}
		}
		process_insertion_candidate_entry(*it, updated_pos);
	}
	set_column_widths();
	return;
}

void
EntryListCtrl::update_for_new(Account const& p_account)
{
	(void)p_account;  // Silence compiler re. unused parameter.
	// Nothing to do.
	return;
}

void
EntryListCtrl::update_for_amended(Account const& p_account)
{
	do_update_for_amended(p_account);
	return;
}

void
EntryListCtrl::update_for_deleted(vector<Entry::Id> const& p_doomed_ids)
{
	vector<Entry::Id>::const_iterator it = p_doomed_ids.begin();
	vector<Entry::Id>::const_iterator const end = p_doomed_ids.end();
	for ( ; it != end; ++it) remove_if_present(*it);
	return;
}

void
EntryListCtrl::selected_entries(vector<Entry>& out)
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if (GetItemState(i, wxLIST_STATE_SELECTED))
		{
			Entry const entry(m_database_connection, GetItemData(i));
			out.push_back(entry);
		}
	}
	return;
}

void
EntryListCtrl::scroll_to_bottom()
{
	int const count = GetItemCount();
	if (count > 0)
	{
		EnsureVisible(count - 1);
	}
	return;
}

vector<SummaryDatum> const&
EntryListCtrl::summary_data() const
{
	return do_get_summary_data();
}

int
EntryListCtrl::num_columns() const
{
	return do_get_num_columns();
}

int
EntryListCtrl::scrollbar_width_allowance() const
{
	// TODO Make this more precise
	return 50;
}

EntryReader*
EntryListCtrl::do_make_entry_reader() const
{
	return new ActualOrdinaryEntryReader(m_database_connection);
}

vector<SummaryDatum> const&
EntryListCtrl::do_get_summary_data() const
{
	static const vector<SummaryDatum> ret;
	assert (ret.empty());
	return ret;
}

void
EntryListCtrl::do_initialize_summary_data()
{
	// Do nothing.
	return;
}

void
EntryListCtrl::do_process_candidate_entry_for_summary(Entry const& p_entry)
{
	(void)p_entry;  // Silence compiler re. unused parameter.
	return;
}

void
EntryListCtrl::do_process_removal_for_summary(long p_row)
{
	(void)p_row;  // Silence compiler re. unused parameter.
	return;
}


int
EntryListCtrl::date_col_num() const
{
	return 0;
}

void
EntryListCtrl::push_back_entry(Entry const& p_entry)
{
	long const i = GetItemCount();
	assert (date_col_num() == 0);
	InsertItem(i, date_format_wx(p_entry.date()));
	do_set_non_date_columns(i, p_entry);

	// The item may change position due to e.g. sorting, so store the
	// Entry ID in the item's data
	// TODO Do a static assert to ensure second param will fit the id.
	SetItemData(i, p_entry.id());
	m_id_set.insert(p_entry.id());
	return;
}

void
EntryListCtrl::insert_entry(Entry const& p_entry, long p_row)
{
	gregorian::date const date = p_entry.date();
	assert (p_row >= -1);
	long const pos = ((p_row == -1)? row_for_date(date): p_row);
	InsertItem(pos, date_format_wx(date));
	do_set_non_date_columns(pos, p_entry);
	Entry::Id const id = p_entry.id();
	SetItemData(pos, id);
	m_id_set.insert(id);
	return;
}

void
EntryListCtrl::remove_if_present(Entry::Id p_entry_id)
{
	IdSet::const_iterator const it = m_id_set.find(p_entry_id);
	if (it != m_id_set.end())
	{
		long const pos = FindItem(-1, p_entry_id);
		assert (GetItemData(pos) == static_cast<unsigned long>(p_entry_id));
		do_process_removal_for_summary(pos);
		DeleteItem(pos);
		m_id_set.erase(it);
	}
	return;
}

PhatbooksDatabaseConnection&
EntryListCtrl::database_connection()
{
	return m_database_connection;
}

PhatbooksDatabaseConnection const&
EntryListCtrl::database_connection() const
{
	return m_database_connection;
}

}  // namespace gui
}  // namespace phatbooks

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
#include "phatbooks_database_connection.hpp"
#include "pl_account_entry_list_ctrl.hpp"
#include "unfiltered_entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
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

void
EntryListCtrl::initialize(EntryListCtrl* p_entry_list_ctrl)
{
	p_entry_list_ctrl->insert_columns();
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
	do_initialize_accumulation(it, end);
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

long
EntryListCtrl::row_for_date(gregorian::date const& p_date)
{
	long min = 0;
	long max = GetItemCount();
	while (true)
	{
		// WARNING Should I be getting the dates from the text in the
		// EntryListCtrl row, rather than from the Entry referenced by the
		// row? They should usually be the same, but perhaps they won't always
		// be the same - especially if we are in the process of updating
		// for new or changed Entries.
		long const guess = (max + min) / 2;
		if (guess == 0)
		{
			return guess;
		}
		Entry::Id const entry_id = GetItemData(guess);
		Entry const entry(m_database_connection, entry_id);
		gregorian::date const date = entry.date();
	
		long const guess_predecessor = guess - 1;
		assert (guess_predecessor >= 0);
		Entry::Id const predecessor_entry_id = GetItemData(guess_predecessor);
		Entry const predecessor_entry(database_connection(), predecessor_entry_id);
		gregorian::date const predecessor_date = predecessor_entry.date();

		assert (predecessor_date <= date);
		if ((predecessor_date <= p_date) && (p_date <= date))
		{
			return guess;
		}
		if (p_date < predecessor_date)
		{
			max = guess_predecessor;
		}
		else if (p_date > date)
		{
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
	if (do_approve_entry(p_entry)) push_back_entry(p_entry);
	return;
}

void
EntryListCtrl::process_insertion_candidate_entry(Entry const& p_entry)
{
	assert (p_entry.has_id());
	if (do_approve_entry(p_entry)) insert_entry(p_entry);
	return;
}

void
EntryListCtrl::do_update_for_amended(Account const& p_account)
{
	(void)p_account;  // Silence compiler re. unused variable
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
		for ( ; it != end; ++it)
		{
			if (do_approve_entry(*it))
			{
				insert_entry(*it);
			}
		}
	}
	set_column_widths();
	return;
}

void
EntryListCtrl::update_for_amended(OrdinaryJournal const& p_journal)
{
	// WARNING This has the potential to cause serious flicker.
	// Under Windows we could do freeze/thaw.
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
		Entry::Id const id = it->id();
		IdSet::const_iterator const jt = m_id_set.find(id);
		if (jt != m_id_set.end())
		{
			// Entry is displayed - delete it first - then we will reinsert
			// it at the correct date position (note date may have changed)
			// if still approved.
			long const pos = FindItem(-1, id);
			assert (GetItemData(pos) == static_cast<unsigned long>(it->id()));
			DeleteItem(pos);
			m_id_set.erase(jt);
		}
		process_insertion_candidate_entry(*it);
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
	for ( ; it != end; ++it)
	{
		IdSet::iterator jt = m_id_set.find(*it);
		if (jt != m_id_set.end())
		{
			DeleteItem(FindItem(-1, *jt));
			m_id_set.erase(jt);
		}
	}
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
	do_accumulate(p_entry);
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
EntryListCtrl::insert_entry(Entry const& p_entry)
{
	gregorian::date const date = p_entry.date();
	long const pos = row_for_date(date);
	InsertItem(pos, date_format_wx(date));
	do_set_non_date_columns(pos, p_entry);
	Entry::Id const id = p_entry.id();
	SetItemData(pos, id);
	m_id_set.insert(id);
	return;
}

void
EntryListCtrl::do_accumulate(Entry const& p_entry)
{
	(void)p_entry;  // Silence compiler re. unused variable
	return;
}

void
EntryListCtrl::do_initialize_accumulation
(	EntryReader::const_iterator it,
	EntryReader::const_iterator const& end
)
{
	// Silence compiler re. unused parameters
	(void)it;
	(void)end;
	return;
}

PhatbooksDatabaseConnection&
EntryListCtrl::database_connection()
{
	return m_database_connection;
}

}  // namespace gui
}  // namespace phatbooks

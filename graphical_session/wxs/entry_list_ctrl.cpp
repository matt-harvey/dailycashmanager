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
	PhatbooksDatabaseConnection
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
	case account_super_type::balance_sheet
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
	do_insert_columns();
	return;
}

void
EntryListCtrl::populate()
{
	boost::scoped_ptr<EntryReader> const reader(make_entry_reader());
	EntryReader::const_iterator it = reader->begin();
	EntryReader::const_iterator const end = reader->end();
	if (do_require_progress_log())
	{
		EntryReader::size_type i = 0;
		EntryReader::size_type progress = 0;
		EntryReader::size_type const progress_scaling_factor = 32;
		EntryReader::size_type const progress_max =
			reader.size() / progress_scaling_factor;
		wxProgressDialog progress_dialog
		(	wxEmptyString,
			"Loading transactions...",
			progress_max,
			this,
			wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxRESIZE_BORDER
		);
		for ( ; it != end; ++it, ++i)
		{
			process_candidate_entry(*it);
			if (i % progress_scaling_factor == 0)
			{
				assert (progress <= progress_max);
				progress_dialog.Update(progress);'
				++progress;
			}
		}
		progress_dialog.Destroy();
	}
	else
	{
		for ( ; it != end; ++it)
		{
			process_candidate_entry(*it);
		}
	}
	return;
}

void
EntryListCtrl::set_column_widths()
{
	do_set_column_widths();
	return;
}

void
EntryListCtrl::process_candidate_entry(Entry const& p_entry)
{
	assert (entry.has_id());
	if (do_approve_entry(p_entry))
	{
		long const i = GetItemCount();
		do_push_entry(p_entry);
		// The item may change position due to e.g. sorting, so store the
		// Entry ID in the item's data
		// TODO Do a static assert to ensure second param will fit the id.
		SetItemData(i, entry.id());
		m_id_set.insert(entry.id());
	}
	return;
}

void
EntryListCtrl::do_update_for_amended(Account const& p_account)
{
	(void)p_account;  // Silence compiler re. unused variable
	return;
}

void
EntryListCtrl::update_row_for_entry(long p_row, Entry const& p_entry)
{
	assert (entry.has_id());
	assert (do_approve_entry(p_entry));
	assert (FindItem(-1, p_entry.id()) == p_row);
	do_update_row_for_entry();
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
		for ( ; it != end; ++it) process_candidate_entry(*it);
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
		Entry::Id const id = it->id();
		IdSet::const_iterator const jt = m_id_set.find(id);
		if (jt == m_id_set.end())
		{
			// Entry not yet displayed.
			process_candidate_entry(*it);
		}
		else
		{
			// Entry is displayed
			long const pos = FindItem(-1, id);
			assert (GetItemData(pos) == static_cast<unsigned long>(it->id()));
			if (!do_approve_entry(*it))
			{
				// Things have changed such that the Entry should no longer be
				// included in the display.
				DeleteItem(pos);
				assert (jt != m_id_set.end());
				m_id_set.erase(jt);
			}
			else
			{
				update_row_for_entry(pos, *it);
			}
		}
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


}  // namespace gui
}  // namespace phatbooks

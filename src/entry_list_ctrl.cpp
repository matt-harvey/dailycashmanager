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


#include "gui/entry_list_ctrl.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "date.hpp"
#include "date_parser.hpp"
#include "entry.hpp"
#include "entry_table_iterator.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "gui/app.hpp"
#include "gui/bs_account_entry_list_ctrl.hpp"
#include "gui/locale.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/pl_account_entry_list_ctrl.hpp"
#include "gui/reconciliation_entry_list_ctrl.hpp"
#include "gui/summary_datum.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/progdlg.h>
#include <wx/scrolwin.h>
#include <string>
#include <type_traits>
#include <vector>

using boost::lexical_cast;
using boost::optional;
using jewel::value;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::is_signed;
using std::pair;
using std::string;
using std::unique_ptr;
using std::vector;

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

EntryListCtrl::~EntryListCtrl()
{
}

EntryListCtrl*
EntryListCtrl::create_actual_ordinary_entry_list
(	wxWindow* p_parent,
	wxSize const& p_size,
	Handle<Account> const& p_account,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
)
{
	EntryListCtrl* ret = 0;
	switch (super_type(p_account->account_type()))
	{
	case AccountSuperType::balance_sheet:
		ret = new BSAccountEntryListCtrl
		(	p_parent,
			p_size,
			p_account,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	case AccountSuperType::pl:
		ret = new PLAccountEntryListCtrl
		(	p_parent,
			p_size,
			p_account,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	default:
		JEWEL_HARD_ASSERT (false);  // Execution never reaches here.
	}
	JEWEL_ASSERT (ret);
	initialize(ret);
	return ret;
}

ReconciliationEntryListCtrl*
EntryListCtrl::create_reconciliation_entry_list
(	ReconciliationListPanel* p_parent,
	wxSize const& p_size,
	Handle<Account> const& p_account,
	gregorian::date const& p_min_date,
	gregorian::date const& p_max_date
)
{
	JEWEL_ASSERT
	(	super_type(p_account->account_type()) ==
		AccountSuperType::balance_sheet
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
	unique_ptr<SQLStatement> statement = do_create_entry_selector();
	while (statement->step())
	{
		process_push_candidate_entry
		(	Handle<Entry>
			(	database_connection(),
				statement->extract<Id>(0)
			)
		);
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
EntryListCtrl::date_displayed(long p_row, DateParser const& p_parser) const
{
	JEWEL_ASSERT (date_col_num() == 0);
	optional<gregorian::date> const maybe_date =
		p_parser.parse(GetItemText(p_row));
	JEWEL_ASSERT (maybe_date);
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
	JEWEL_ASSERT (max == num_rows);
	JEWEL_ASSERT (max > 0);
	DateParser const parser;
	if (date_displayed(max - 1, parser) <= p_date)
	{
		// Very end
		return max;
	}
	// Date is neither first nor last... find it using binary search.
	while (true)
	{
		JEWEL_ASSERT (min < max);
		long guess = (min + max) / 2;
		if (guess == min)
		{
			// Find end of contiguous rows showing this date
			while ((date_displayed(guess, parser) == p_date) && (guess != num_rows))
			{
				++guess;
			}
			return guess;
		}
		gregorian::date const date = date_displayed(guess, parser);
		JEWEL_ASSERT (guess > 0);
		gregorian::date const predecessor_date = date_displayed(guess - 1, parser);
		JEWEL_ASSERT (predecessor_date <= date);
		if ((predecessor_date <= p_date) && (p_date <= date))
		{
			// Find end of contiguous rows showing this date
			while ((date_displayed(guess, parser) == p_date) && (guess != num_rows))
			{
				++guess;
			}
			return guess;
		}
		JEWEL_ASSERT ((p_date < predecessor_date) || (date < p_date));
		if (p_date < predecessor_date)
		{
			JEWEL_ASSERT (guess < max);
			max = guess;
		}
		else if (date < p_date)
		{
			JEWEL_ASSERT (guess > min);
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
EntryListCtrl::process_push_candidate_entry(Handle<Entry> const& p_entry)
{
	JEWEL_ASSERT (p_entry->has_id());
	do_process_candidate_entry_for_summary(p_entry);
	if (do_approve_entry(p_entry)) push_back_entry(p_entry);
	return;
}

void
EntryListCtrl::process_insertion_candidate_entry
(	Handle<Entry> const& p_entry,
	long p_row
)
{
	JEWEL_ASSERT (p_entry->has_id());
	do_process_candidate_entry_for_summary(p_entry);
	if (do_approve_entry(p_entry)) insert_entry(p_entry, p_row);
	return;
}

void
EntryListCtrl::do_update_for_amended(Handle<Account> const& p_account)
{
	(void)p_account;  // Silence compiler re. unused variable
	return;
}

void
EntryListCtrl::on_item_activated(wxListEvent& event)
{
	Handle<Entry> const entry
	(	database_connection(),
		GetItemData(event.GetIndex())
	);

	// Fire a PersistentJournal editing request. This will be handled
	// higher up the window hierarchy.
	PersistentObjectEvent::fire
	(	this,
		PHATBOOKS_JOURNAL_EDITING_EVENT,
		entry->journal_id()
	);
	return;
}

void
EntryListCtrl::update_for_new(Handle<OrdinaryJournal> const& p_journal)
{
	if (p_journal->is_actual())
	{
		for (Handle<Entry> const& entry: p_journal->entries())
		{
			process_insertion_candidate_entry(entry);
		}
	}
	set_column_widths();
	return;
}

void
EntryListCtrl::update_for_amended(Handle<OrdinaryJournal> const& p_journal)
{
	if (!p_journal->is_actual())
	{
		return;
	}
	JEWEL_ASSERT (p_journal->is_actual());
	wxString const wx_date_string = date_format_wx(p_journal->date());
	DateParser const parser;
	for (Handle<Entry> const& entry: p_journal->entries())
	{
		long updated_pos = -1;
		JEWEL_ASSERT (entry->has_id());
		IdSet::const_iterator const jt = m_id_set.find(entry->id());
		if (jt != m_id_set.end())
		{
			long const pos = FindItem(-1, entry->id());
			JEWEL_ASSERT
			(	GetItemData(pos) ==
				static_cast<unsigned long>(entry->id())
			);
			gregorian::date const old_date = date_displayed(pos, parser);
			do_process_removal_for_summary(pos);
			DeleteItem(pos);
			m_id_set.erase(jt);
			if (old_date == entry->date())
			{
				updated_pos = pos;	
			}
		}
		process_insertion_candidate_entry(entry, updated_pos);
	}
	set_column_widths();
	return;
}

void
EntryListCtrl::update_for_new(Handle<Account> const& p_account)
{
	(void)p_account;  // Silence compiler re. unused parameter.
	// Nothing to do.
	return;
}

void
EntryListCtrl::update_for_amended(Handle<Account> const& p_account)
{
	do_update_for_amended(p_account);
	return;
}

void
EntryListCtrl::update_for_deleted(vector<sqloxx::Id> const& p_doomed_ids)
{
	for (sqloxx::Id const doomed_id: p_doomed_ids)
	{
		remove_if_present(doomed_id);
	}
	return;
}

void
EntryListCtrl::selected_entries(vector<Handle<Entry> >& out)
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	for ( ; i != lim; ++i)
	{
		if (GetItemState(i, wxLIST_STATE_SELECTED))
		{
			Handle<Entry> const entry(m_database_connection, GetItemData(i));
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
	// TODO LOW PRIORITY Make this more precise
	return 50;
}

vector<SummaryDatum> const&
EntryListCtrl::do_get_summary_data() const
{
	static const vector<SummaryDatum> ret;
	JEWEL_ASSERT (ret.empty());
	return ret;
}

void
EntryListCtrl::do_initialize_summary_data()
{
	// Do nothing.
	return;
}

void
EntryListCtrl::do_process_candidate_entry_for_summary
(	Handle<Entry> const& p_entry
)
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
EntryListCtrl::push_back_entry(Handle<Entry> const& p_entry)
{
	auto const i = GetItemCount();
	JEWEL_ASSERT (date_col_num() == 0);
	InsertItem(i, date_format_wx(p_entry->date()));
	do_set_non_date_columns(i, p_entry);

	// The item may change position due to e.g. sorting, so store the
	// Entry ID in the item's data
	static_assert
	(	(sizeof(decltype(i)) >= sizeof(decltype(p_entry->id()))) &&
		is_signed<decltype(i)>::value &&
		is_signed<decltype(p_entry->id())>::value,
		"Cannot safely fit values of type of p_entry->id() into the type "
		"of i, in function EntryListCtrl::push_back_entry."
	);
	SetItemData(i, p_entry->id());
	m_id_set.insert(p_entry->id());
	return;
}

void
EntryListCtrl::insert_entry(Handle<Entry> const& p_entry, long p_row)
{
	gregorian::date const date = p_entry->date();
	JEWEL_ASSERT (p_row >= -1);
	long const pos = ((p_row == -1)? row_for_date(date): p_row);
	InsertItem(pos, date_format_wx(date));
	do_set_non_date_columns(pos, p_entry);
	sqloxx::Id const id = p_entry->id();
	SetItemData(pos, id);
	m_id_set.insert(id);
	return;
}

void
EntryListCtrl::remove_if_present(sqloxx::Id p_entry_id)
{
	IdSet::const_iterator const it = m_id_set.find(p_entry_id);
	if (it != m_id_set.end())
	{
		long const pos = FindItem(-1, p_entry_id);
		JEWEL_ASSERT
		(	GetItemData(pos) ==
			static_cast<unsigned long>(p_entry_id)
		);
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

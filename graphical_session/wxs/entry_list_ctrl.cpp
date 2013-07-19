// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "entry_list_ctrl.hpp"
#include "account.hpp"
#include "app.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "entry_reader.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/progdlg.h>
#include <vector>
#include <string>

using boost::lexical_cast;
using jewel::value;
using std::string;
using std::vector;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

// Anonymous namespace
namespace
{
	int date_col_num()
	{
		return 0;
	}
	int account_col_num()
	{
		return 1;
	}
	int comment_col_num()
	{
		return 2;
	}
	int amount_col_num()
	{
		return 3;
	}
	int reconciled_col_num()
	{
		return 4;
	}

}  // End anonymous namespace


EntryListCtrl*
EntryListCtrl::create_actual_ordinary_entry_list
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
)
{
	ActualOrdinaryEntryReader const reader(p_database_connection);
	EntryListCtrl* ret =
		new EntryListCtrl(p_parent, p_size, reader, p_database_connection);
	return ret;
}

EntryListCtrl*
EntryListCtrl::create_actual_ordinary_entry_list
(	wxWindow* p_parent,
	wxSize const& p_size,
	Account const& p_account
)
{
	EntryListCtrl* ret = new EntryListCtrl(p_parent, p_size, p_account);
	return ret;
}

EntryListCtrl::EntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	EntryReader const& p_reader,
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
	assert (!m_maybe_account);
	insert_columns();

	EntryReader::size_type i = 0;
	EntryReader::size_type progress = 0;
	EntryReader::size_type const progress_scaling_factor = 32;
	EntryReader::size_type const progress_max =
		p_reader.size() / progress_scaling_factor;

	// Create a progress dialog
	wxProgressDialog progress_dialog
	(	wxEmptyString,
		"Loading transactions...",
		progress_max,
		this,
		wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxRESIZE_BORDER
	);

	gregorian::date const opening_balance_journal_date =
		m_database_connection.opening_balance_journal_date();
	for
	(	EntryReader::const_iterator it = p_reader.begin(),
			end = p_reader.end();
		it != end;
		++it, ++i
	)
	{
		if (it->date() != opening_balance_journal_date)
		{
			add_entry(*it);
		}

		// Update the progress dialog
		if (i % progress_scaling_factor == 0)
		{
			assert (progress <= progress_max);
			progress_dialog.Update(progress);
			++progress;
		}
	}
	progress_dialog.Destroy();
}

EntryListCtrl::EntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Account const& p_account
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		p_size,
		wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_account.database_connection()),
	m_maybe_account(p_account)
{
	insert_columns();

	// TODO Should we bother with a progress indicator here?
	
	gregorian::date const opening_balance_journal_date =
		m_database_connection.opening_balance_journal_date();
	ActualOrdinaryEntryReader const reader(m_database_connection);
	ActualOrdinaryEntryReader::const_iterator it = reader.begin();
	ActualOrdinaryEntryReader::const_iterator const end = reader.end();
	for ( ; it != end; ++it)
	{
		if
		(	(it->account() == p_account) &&
			(it->date() != opening_balance_journal_date)
		)
		{
			add_entry(*it);
		}
	}
	set_column_widths();
}

void
EntryListCtrl::insert_columns()
{
	InsertColumn(date_col_num(), "Date", wxLIST_FORMAT_RIGHT);
	InsertColumn(account_col_num(), "Account", wxLIST_FORMAT_LEFT);
	InsertColumn(comment_col_num(), "Comment", wxLIST_FORMAT_LEFT);
	InsertColumn(amount_col_num(), "Amount", wxLIST_FORMAT_RIGHT);
	InsertColumn(reconciled_col_num(), "R", wxLIST_FORMAT_LEFT);
	return;
}

void
EntryListCtrl::set_column_widths()
{
	for (int j = 0; j != 5; ++j)
	{
		SetColumnWidth(j, wxLIST_AUTOSIZE);
	}
	SetColumnWidth
	(	comment_col_num(),
		GetColumnWidth(account_col_num())
	);
	return;
}

void
EntryListCtrl::add_entry(Entry const& entry)
{
	assert (entry.has_id());  // assert precondition
	assert
	(	!filtering_for_account() ||
		(entry.account() == value(m_maybe_account))
	);

	OrdinaryJournal journal(entry.journal<OrdinaryJournal>());
	wxString const wx_date_string = date_format_wx(journal.date());
	wxString const account_string = bstring_to_wx(entry.account().name());
	wxString const comment_string = bstring_to_wx(entry.comment());
	wxString const amount_string =
		finformat_wx(entry.amount(), locale(), false);

	// TODO Should have a tick icon here rather than a "Y".
	wxString const reconciled_string = (entry.is_reconciled()? "Y": "N");

	long const i = GetItemCount();

	// Populate 0th column
	assert (date_col_num() == 0);
	InsertItem(i, wx_date_string);

	// The item may change position due to e.g. sorting, so store the
	// Entry ID in the item's data
	// TODO Do a static assert to ensure second param will fit the id.
	assert (entry.has_id());
	SetItemData(i, entry.id());
	m_id_set.insert(entry.id());

	// Populate the other columns
	SetItem(i, account_col_num(), account_string);
	SetItem(i, comment_col_num(), comment_string);
	SetItem(i, amount_col_num(), amount_string);
	SetItem(i, reconciled_col_num(), reconciled_string);
}

bool
EntryListCtrl::filtering_for_account() const
{
	return static_cast<bool>(m_maybe_account);
}

void
EntryListCtrl::update_for_new(OrdinaryJournal const& p_journal)
{
	if (p_journal.is_actual())
	{
		vector<Entry>::const_iterator it = p_journal.entries().begin();
		vector<Entry>::const_iterator const end = p_journal.entries().end();
		if (!filtering_for_account())
		{
			for ( ; it != end; ++it) add_entry(*it);
		}
		else
		{
			assert (m_maybe_account);
			Account const account = value(m_maybe_account);
			for ( ; it != end; ++it)
			{
				if (it->account() == account) add_entry(*it);
			}
		}
	}
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
			if
			(	!filtering_for_account() ||
				(it->account() == value(m_maybe_account))
			)
			{
				add_entry(*it);
			}
		}
		else
		{
			// Entry is displayed
			long const pos = FindItem(-1, id);
			assert (GetItemData(pos) == static_cast<unsigned long>(it->id()));
			if
			(	filtering_for_account() &&
				(it->account() != value(m_maybe_account))
			)
			{
				// Account is no longer the filtered-for Account, so remove
				// this Entry from the display.
				DeleteItem(pos);
				assert (jt != m_id_set.end());
				m_id_set.erase(jt);
			}
			else
			{
				// Update the row for this Entry to match the current
				// state of the Entry.
				SetItemText(pos, wx_date_string);
				SetItem
				(	pos,
					account_col_num(),
					bstring_to_wx(it->account().name())
				);
				SetItem
				(	pos,
					comment_col_num(),
					bstring_to_wx(it->comment())
				);
				SetItem
				(	pos,
					amount_col_num(),
					finformat_wx(it->amount(), locale(), false)
				);
				SetItem
				(	pos,
					reconciled_col_num(),
					(it->is_reconciled()? "Y": "N")
				);
			}
		}
	}
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

}  // namespace gui
}  // namespace phatbooks

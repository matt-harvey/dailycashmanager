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
#include <boost/lexical_cast.hpp>
#include <jewel/debug_log.hpp>
#include <wx/gdicmn.h>
#include <wx/progdlg.h>
#include <vector>
#include <string>

using boost::lexical_cast;
using std::string;
using std::vector;

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
(	wxWindow* parent,
	PhatbooksDatabaseConnection& dbc
)
{
	ActualOrdinaryEntryReader const reader(dbc);
	EntryListCtrl* ret = new EntryListCtrl(parent, reader, dbc);
	return ret;
}


EntryListCtrl::EntryListCtrl
(	wxWindow* p_parent,
	EntryReader const& p_reader,
	PhatbooksDatabaseConnection& p_database_connection	
):
	wxListCtrl
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		wxSize
		(	p_parent->GetClientSize().GetX(),
			p_parent->GetClientSize().GetY()
		),
		wxLC_REPORT | wxFULL_REPAINT_ON_RESIZE
	),
	m_database_connection(p_database_connection)
{
	// Insert columns
	InsertColumn(date_col_num(), "Date", wxLIST_FORMAT_RIGHT);
	InsertColumn(account_col_num(), "Account", wxLIST_FORMAT_LEFT);
	InsertColumn(comment_col_num(), "Comment", wxLIST_FORMAT_LEFT);
	InsertColumn(amount_col_num(), "Amount", wxLIST_FORMAT_RIGHT);
	InsertColumn(reconciled_col_num(), "R", wxLIST_FORMAT_LEFT);

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

	for
	(	EntryReader::const_iterator it = p_reader.begin(),
			end = p_reader.end();
		it != end;
		++it, ++i
	)
	{
		add_entry(*it);

		// Update the progress dialog
		if (i % progress_scaling_factor == 0)
		{
			assert (progress <= progress_max);
			progress_dialog.Update(progress);
			++progress;
		}
	}
	progress_dialog.Destroy();
	for (int j = 0; j != 5; ++j)
	{
		SetColumnWidth(j, wxLIST_AUTOSIZE);
	}
	SetColumnWidth
	(	comment_col_num(),
		GetColumnWidth(account_col_num())
	);
}


void
EntryListCtrl::add_entry(Entry const& entry)
{
	assert (entry.has_id());  // assert precondition

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

	// Populate the other columns
	SetItem(i, account_col_num(), account_string);
	SetItem(i, comment_col_num(), comment_string);
	SetItem(i, amount_col_num(), amount_string);
	SetItem(i, reconciled_col_num(), reconciled_string);
}


void
EntryListCtrl::update_for_posted_journal(OrdinaryJournal const& journal)
{
	if (journal.is_actual())
	{
		vector<Entry>::const_iterator it = journal.entries().begin();
		vector<Entry>::const_iterator const end = journal.entries().end();
		for ( ; it != end; ++it)
		{
			add_entry(*it);
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

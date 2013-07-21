#include "unfiltered_entry_list_ctrl.hpp"
#include "entry_list_ctrl.hpp"
#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/window.h>
#include <wx/gdicmn.h>

namespace phatbooks
{
namespace gui
{

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

}  // end anonymous namespace

UnfilteredEntryListCtrl::UnfilteredEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
):
	EntryListCtrl(p_parent, p_size, p_database_connection)
{
}

UnfilteredEntryListCtrl::~UnfilteredEntryListCtrl()
{
}

void
UnfilteredEntryListCtrl::do_require_progress_log()
{
	return true;
}

void
UnfilteredEntryListCtrl::do_insert_columns()
{
	InsertColumn(date_col_num(), "Date", wxLIST_FORMAT_RIGHT);
	InsertColumn(account_col_num(), "Account", wxLIST_FORMAT_LEFT);
	InsertColumn(comment_col_num(), "Memo", wxLIST_FORMAT_LEFT);
	InsertColumn(amount_col_num(), "Amount", wxLIST_FORMAT_RIGHT);
	return;
}

bool
UnfilteredEntryListCtrl::do_approve_entry(Entry const& p_entry) const
{
	return true;
}

void
UnfilteredEntryListCtrl::do_push_entry(Entry const& p_entry)
{
	OrdinaryJournal const journal(entry.journal<OrdinaryJournal>());
	wxString const wx_date_string = date_format_wx(journal.date());
	wxString const account_string = bstring_to_wx(entry.account().name());
	wxString const comment_string = bstring_to_wx(entry.comment());
	wxString const amount_string =
		finformat_wx(entry.amount(), locale(), false);

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

	return;
}


}  // namespace gui
}  // namespace phatbooks

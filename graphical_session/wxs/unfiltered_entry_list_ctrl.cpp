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
	int num_columns()
	{
		return 4;
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

void
UnfilteredEntryListCtrl::do_set_column_widths()
{
	// We arrange the widths so that
	// the Account column takes up just enough size for the
	// Account name - up to a reasonable maximum - the other columns take up
	// just enough room for their contents, and then the comment column
	// is sized such that the total width of all columns occupies exactly
	// the full width of the available area.
	int const num_cols = num_columns();
	for (int j = 0; j != num_cols; ++j)
	{
		SetColumnWidth(j, wxLIST_AUTOSIZE);
	}
	int const max_account_col_width = 200;
	if (GetColumnWidth(account_col_num()) > max_account_col_width)
	{
		SetColumnWidth(account_col_num(), max_account_col_width);
	}
	int total_widths = 0;
	for (int j = 0; j != num_cols; ++j)
	{
		total_widths += GetColumnWidth(j);
	}

	// TODO Make this more precise
	int const scrollbar_width_allowance = 50;

	int const shortfall =
		GetSize().GetWidth() - total_widths - scrollbar_width_allowance;
	int const current_comment_width = GetColumnWidth(comment_col_num());
	SetColumnWidth(comment_col_num(), current_comment_width + shortfall);
	return;
}


}  // namespace gui
}  // namespace phatbooks

#include "unfiltered_entry_list_ctrl.hpp"
#include "entry_list_ctrl.hpp"
#include "entry_reader.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
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
	int anon_num_columns()
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

bool
UnfilteredEntryListCtrl::do_require_progress_log() const
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
	(void)p_entry;  // Silence compiler re. unused parameter.
	return true;
}

void
UnfilteredEntryListCtrl::do_push_entry(Entry const& p_entry)
{
	long const i = GetItemCount();
	OrdinaryJournal const journal(p_entry.journal<OrdinaryJournal>());
	assert (date_col_num() == 0);
	InsertItem(i, date_format_wx(journal.date()));
	set_non_date_columns(i, p_entry);
	return;
}

void
UnfilteredEntryListCtrl::do_update_row_for_entry
(	long p_row,
	Entry const& p_entry
)
{
	OrdinaryJournal const journal(p_entry.journal<OrdinaryJournal>());
	SetItemText(p_row, date_format_wx(journal.date()));
	set_non_date_columns(p_row, p_entry);
	return;
}

void
UnfilteredEntryListCtrl::set_non_date_columns
(	long p_row,
	Entry const& p_entry
)
{
	SetItem
	(	p_row,
		account_col_num(),
		bstring_to_wx(p_entry.account().name())
	);
	SetItem
	(	p_row,
		comment_col_num(),
		bstring_to_wx(p_entry.comment())
	);
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx(p_entry.amount(), locale(), false)
	);
	assert (num_columns() == 4);
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

	int const shortfall =
		GetSize().GetWidth() - total_widths - scrollbar_width_allowance();
	int const current_comment_width = GetColumnWidth(comment_col_num());
	SetColumnWidth(comment_col_num(), current_comment_width + shortfall);
	return;
}

int
UnfilteredEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}

void
UnfilteredEntryListCtrl::do_update_for_amended(Account const& p_account)
{
	size_t i = 0;
	size_t const lim = GetItemCount();
	wxString const name = bstring_to_wx(p_account.name());
	for ( ; i != lim; ++i)
	{
		Entry const entry(database_connection(), GetItemData(i));
		if (entry.account() == p_account)
		{
			SetItem(i, account_col_num(), name);
		}
	}
	return;
}

}  // namespace gui
}  // namespace phatbooks

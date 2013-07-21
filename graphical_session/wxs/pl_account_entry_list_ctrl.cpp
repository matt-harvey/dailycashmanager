#include "pl_account_entry_list_ctrl.hpp"
#include "account_entry_list_ctrl.hpp"
#include "entry.hpp"

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
	int comment_col_num()
	{
		return 1;
	}
	int amount_col_num()
	{
		return 2;
	}
	int anon_num_columns()
	{
		return 3;
	}

}  // end anonymous namespace


PLAccountEntryListCtrl::PLAccountEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Account const& p_account,
	boost::optional<gregorian::date> const& p_maybe_min_date,
	boost::optional<gregorian::date> const& p_maybe_max_date
):
	AccountEntryListCtrl
	(	p_parent,
		p_size,
		p_account,
		p_maybe_min_date,
		p_maybe_max_date
	)
{
}

PLAccountEntryListCtrl::~PLAccountEntryListCtrl()
{
}

void
BSAccountEntryListCtrl::set_non_date_columns(long p_row, Entry const& p_entry)
{
	SetItem
	(	p_row,
		comment_col_num(),
		bstring_to_wx(it->comment())
	);
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx(it->amount(), locale(), false)
	);
	assert (num_columns() == 3);
	return;
}

void
PLAccountEntryListCtrl::do_insert_columns()
{
	InsertColumn(date_col_num(), wxString("Date"), wxLIST_FORMAT_RIGHT);
	InsertColumn(comment_col_num(), wxString("Memo"), wxLIST_FORMAT_LEFT);
	InsertColumn(amount_col_num(), wxString("Amount"), wxLIST_FORMAT_RIGHT);
	assert (num_columns() == 3);
	return;
}

void
PLAccountEntryListCtrl::do_push_entry(Entry const& p_entry)
{
	OrdinaryJournal const journal(entry.journal<OrdinaryJournal>());
	long const i = GetItemCount();

	// Populate 0th column

	assert (date_col_num() == 0);
	InsertItem(i, date_format_wx(journal.date()));

	// Populate the other columns

	SetItem(i, comment_col_num(), bstring_to_wx(entry.comment());

	wxString const amount_string =
		finformat_wx(entry.amount(), locale(), false);
	SetItem(i, amount_col_num(), amount_string);

	return;

}

void
PLAccountEntryListCtrl::do_update_row_for_entry
(	long p_row,
	Entry const& p_entry
)
{
	OrdinaryJournal const journal(entry.journal<OrdinaryJournal>());
	SetItemText(p_row, date_format_wx(journal.date()));
	set_non_date_columns(p_row, p_entry);
	return;
}

int
PLAccountEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}

}  // namespace gui
}  // namespace phatbooks

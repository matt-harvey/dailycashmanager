#include "bs_account_entry_list_ctrl.hpp"
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
	int reconciled_col_num()
	{
		return 3;
	}
	int anon_num_columns()
	{
		return 4;
	}

}  // end anonymous namespace


BSAccountEntryList::BSAccountEntryList
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

BSAccountEntryListCtrl::~BSAccountEntryListCtrl()
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
	SetItem
	(	p_row,
		reconciled_col_num(),
		(it->is_reconciled? wxString("Y"): wxString("N"))
	);
	assert (num_columns() == 4);
	return;
}

void
BSAccountEntryListCtrl::do_insert_columns()
{
	InsertColumn(date_col_num(), wxString("Date"), wxLIST_FORMAT_RIGHT);
	InsertColumn(comment_col_num(), wxString("Memo"), wxLIST_FORMAT_LEFT);
	InsertColumn(amount_col_num(), wxString("Amount"), wxLIST_FORMAT_RIGHT);
	InsertColumn(reconciled_col_num(), wxString("R"), wxLIST_FORMAT_LEFT);
	assert (num_columns() == 4);
	return;
}

void
BSAccountEntryListCtrl::do_push_entry(Entry const& p_entry)
{
	long const i = GetItemCount();
	OrdinaryJournal const journal(entry.journal<OrdinaryJournal>());
	assert (date_col_num() == 0);
	InsertItem(i, date_format_wx(journal.date()));
	set_non_date_columns(i, p_entry);
	return;
}

void
BSAccountEntryListCtrl::do_update_row_for_entry
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
BSAccountEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}


}  // namespace gui
}  // namespace phatbooks

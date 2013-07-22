#include "bs_account_entry_list_ctrl.hpp"
#include "entry.hpp"
#include "filtered_entry_list_ctrl.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/optional.hpp>

using boost::optional;
using jewel::value;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

namespace
{
	int comment_col_num()
	{
		return 1;
	}
	int amount_col_num()
	{
		return 2;
	}
	int balance_col_num()
	{
		return 3;
	}
	int reconciled_col_num()
	{
		return 4;
	}
	int reconciled_balance_col_num()
	{
		return 5;
	}
	int anon_num_columns()
	{
		return 6;
	}

}  // end anonymous namespace


BSAccountEntryListCtrl::BSAccountEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Account const& p_account,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
):
	FilteredEntryListCtrl
	(	p_parent,
		p_size,
		p_account,
		p_maybe_min_date,
		p_maybe_max_date
	),
	m_accumulated_balance(0, p_account.commodity().precision()),
	m_accumulated_reconciled_balance(0, p_account.commodity().precision())
{
}

BSAccountEntryListCtrl::~BSAccountEntryListCtrl()
{
}

void
BSAccountEntryListCtrl::do_set_non_date_columns(long p_row, Entry const& p_entry)
{
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
	SetItem
	(	p_row,
		balance_col_num(),
		finformat_wx(m_accumulated_balance, locale(), false)
	);
	SetItem
	(	p_row,
		reconciled_col_num(),
		(p_entry.is_reconciled()? wxString("Y"): wxString("N"))
	);
	SetItem
	(	p_row,
		reconciled_balance_col_num(),
		finformat_wx(m_accumulated_reconciled_balance, locale(), false)
	);
	assert (num_columns() == 6);
	return;
}

void
BSAccountEntryListCtrl::do_accumulate(Entry const& p_entry)
{
	m_accumulated_balance += p_entry.amount();
	if (p_entry.is_reconciled())
	{
		m_accumulated_reconciled_balance += p_entry.amount();
	}
	return;
}

void
BSAccountEntryListCtrl::do_initialize_accumulation
(	EntryReader::const_iterator it,
	EntryReader::const_iterator const& end
)
{
	gregorian::date const start_date = min_date();
	for ( ; (it->date() < start_date) && (it != end); ++it)
	{
		if (it->account() == account()) do_accumulate(*it);
	}
	return;
}

void
BSAccountEntryListCtrl::do_insert_non_date_columns()
{
	InsertColumn
	(	comment_col_num(),
		wxString("Memo"),
		wxLIST_FORMAT_LEFT
	);
	InsertColumn
	(	amount_col_num(),
		wxString("Amount"),
		wxLIST_FORMAT_RIGHT
	);
	InsertColumn
	(	balance_col_num(),
		wxString("Balance"),
		wxLIST_FORMAT_RIGHT
	);
	InsertColumn
	(	reconciled_col_num(),
		wxString("R"),
		wxLIST_FORMAT_LEFT
	);
	InsertColumn
	(	reconciled_balance_col_num(),
		wxString("Reconciled"),
		wxLIST_FORMAT_LEFT
	);
	assert (num_columns() == 6);
	return;
}

int
BSAccountEntryListCtrl::do_get_comment_col_num() const
{
	return comment_col_num();
}

int
BSAccountEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}


}  // namespace gui
}  // namespace phatbooks

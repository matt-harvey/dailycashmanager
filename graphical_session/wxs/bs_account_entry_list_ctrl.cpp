// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "bs_account_entry_list_ctrl.hpp"
#include "account.hpp"
#include "entry_handle.hpp"
#include "filtered_entry_list_ctrl.hpp"
#include "locale.hpp"
#include "ordinary_journal_handle.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>

using boost::optional;
using jewel::value;
using sqloxx::Handle;

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
	int anon_num_columns()
	{
		return 3;
	}

}  // end anonymous namespace


BSAccountEntryListCtrl::BSAccountEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Handle<Account> const& p_account,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
):
	FilteredEntryListCtrl
	(	p_parent,
		p_size,
		p_account,
		p_maybe_min_date,
		p_maybe_max_date
	)
{
}

void
BSAccountEntryListCtrl::do_set_non_date_columns
(	long p_row,
	EntryHandle const& p_entry
)
{
	SetItem
	(	p_row,
		comment_col_num(),
		p_entry->comment()
	);
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx
		(	p_entry->amount(),
			locale(),
			DecimalFormatFlags().clear(string_flags::dash_for_zero)
		)
	);
	JEWEL_ASSERT (num_columns() == 3);
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
	JEWEL_ASSERT (num_columns() == 3);
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

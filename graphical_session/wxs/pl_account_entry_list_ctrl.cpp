#include "pl_account_entry_list_ctrl.hpp"
#include "filtered_entry_list_ctrl.hpp"
#include "entry.hpp"
#include "locale.hpp"
#include "ordinary_journal.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>

using boost::optional;
using jewel::Decimal;
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
	int accumulator_col_num()
	{
		return 3;
	}
	int anon_num_columns()
	{
		return 4;
	}

}  // end anonymous namespace


PLAccountEntryListCtrl::PLAccountEntryListCtrl
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
	m_reverse_signs(p_account.account_type() == account_type::revenue),
	m_accumulator(0, p_account.commodity().precision())
{
}

PLAccountEntryListCtrl::~PLAccountEntryListCtrl()
{
}

void
PLAccountEntryListCtrl::do_set_non_date_columns
(	long p_row,
	Entry const& p_entry
)
{
	SetItem
	(	p_row,
		comment_col_num(),
		bstring_to_wx(p_entry.comment())
	);
	SetItem
	(	p_row,
		amount_col_num(),
		finformat_wx(friendly_amount(p_entry), locale(), false)
	);
	SetItem
	(	p_row,
		accumulator_col_num(),
		finformat_wx(m_accumulator, locale(), false)
	);
	assert (num_columns() == 4);
	return;
}

void
PLAccountEntryListCtrl::do_accumulate(Entry const& p_entry)
{
	m_accumulator += friendly_amount(p_entry);
	return;
}

wxString
PLAccountEntryListCtrl::verb() const
{
	switch (account().account_type())
	{
	case account_type::revenue:
		return wxString("earned");
	case account_type::expense:
		return wxString("spent");
	default:
		assert (false);
	}
}

void
PLAccountEntryListCtrl::do_insert_non_date_columns()
{
	InsertColumn
	(	comment_col_num(),
		wxString("Memo"),
		wxLIST_FORMAT_LEFT
	);
	InsertColumn
	(	amount_col_num(),
		wxString("Amount ") + verb(),
		wxLIST_FORMAT_RIGHT
	);
	InsertColumn
	(	accumulator_col_num(),
		wxString("Total ") + verb(),
		wxLIST_FORMAT_RIGHT
	);
	assert (num_columns() == 4);
	return;
}

int
PLAccountEntryListCtrl::do_get_comment_col_num() const
{
	return comment_col_num();
}

int
PLAccountEntryListCtrl::do_get_num_columns() const
{
	return anon_num_columns();
}

jewel::Decimal
PLAccountEntryListCtrl::friendly_amount(Entry const& p_entry) const
{
	return m_reverse_signs? -p_entry.amount(): p_entry.amount();
}


}  // namespace gui
}  // namespace phatbooks

#include "account_entry_list_ctrl.hpp"
#include "account.hpp"
#include "entry_list_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/window.h>

using boost::optional;
using jewel::value;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

// Anonymous namespace
namespace
{
	bool lies_within
	(	gregorian::date const& p_target,
		gregorian::date const& p_min,
		optional<gregorian::date> const& p_max
	)
	{
		bool const ok_with_min = (p_target >= p_min);
		bool const ok_with_max = (!p_max || (p_target <= value(p_max)));
		return ok_with_min && ok_with_max;
	}

}  // End anonymous namespace

AccountEntryListCtrl::AccountEntryListCtrl
(	wxWindow* p_parent,
	wxSize const& p_size,
	Account const& p_account,
	boost::optional<gregorian::date> const& p_maybe_min_date,
	boost::optional<gregorian::date> const& p_maybe_max_date
):
	EntryListCtrl(p_parent, p_size, p_account.database_connection()),
	m_account(p_account),
	m_min_date
	(	p_maybe_min_date?
		value(p_maybe_min_date):
		p_account.database_connection().opening_balance_journal_date() +
			gregorian::date_duration(1)
	),
	m_maybe_max_date(p_maybe_max_date)
{
}

AccountEntryListCtrl::~AccountEntryListCtrl()
{
}

bool
AccountEntryListCtrl::do_require_progress_log() const
{
	return false;
}

bool
AccountEntryListCtrl::do_approve_entry(Entry const& p_entry) const
{
	return
		(p_entry.account() == m_account) &&
		lies_within(p_entry.date(), m_min_date, m_maybe_max_date);
}

void
AccountEntryListCtrl::do_set_column_widths()
{
	// We arrange the widths so that the comment column
	// is sized such that the total width of all columns occupies exactly
	// the full width of the available area.
	int const num_cols = num_columns();
	for (int j = 0; j != num_cols; ++j)
	{
		SetColumnWidth(j, wxLIST_AUTOSIZE);
	}
	int total_widths = 0;
	for (int j = 0; j != num_cols; ++j)
	{
		total_widths += GetColumnWidth(j);
	}

	int const shortfall =
		GetSize().GetWidth() - total_widths - scrollbar_width_allowance();
	int const current_comment_width =
		GetColumnWidth(do_get_comment_col_num());
	SetColumnWidth
	(	do_get_comment_col_num(),
		current_comment_width + shortfall
	);
	return;
}


}  // namespace gui
}  // namespace phatbooks

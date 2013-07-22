#include "filtered_entry_list_ctrl.hpp"
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

FilteredEntryListCtrl::FilteredEntryListCtrl
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

FilteredEntryListCtrl::~FilteredEntryListCtrl()
{
}

bool
FilteredEntryListCtrl::do_require_progress_log() const
{
	return false;
}

bool
FilteredEntryListCtrl::do_approve_entry(Entry const& p_entry) const
{
	return
		(p_entry.account() == m_account) &&
		lies_within(p_entry.date(), m_min_date, m_maybe_max_date);
}

void
FilteredEntryListCtrl::do_set_column_widths()
{
	autosize_column_widths();
	adjust_comment_column_to_fit();
	return;
}


}  // namespace gui
}  // namespace phatbooks
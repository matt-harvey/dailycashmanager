#include "account_entry_list_ctrl.hpp"

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

AccountEntryListCtrl
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
AccountEntryListCtrl::do_require_progress_log()
{
	return false;
}

bool
AccountEntryListCtrl::do_approve_entry(Entry const& p_entry)
{
	return
		(p_entry.account() == m_account) &&
		lies_within(p_entry.date(), m_min_date, m_maybe_max_date);
}



}  // namespace gui
}  // namespace phatbooks

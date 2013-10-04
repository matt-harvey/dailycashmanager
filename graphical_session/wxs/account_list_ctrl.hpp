// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_list_ctrl_hpp_4892309124065707
#define GUARD_account_list_ctrl_hpp_4892309124065707

#include "account_handle.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include <boost/optional.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/wx.h>
#include <set>

namespace phatbooks
{

class PhatbooksDatabaseConnection;

namespace gui
{

/**
 * Displays a list of Accounts and their balances, and, optionally
 * the daily budget associated with each Account.
 *
 * NOTE The implementation of this class assumes we will never
 * allow Accounts to be deleted. It is unlikely this assumption will
 * be broken - especially as we do allow users to hide Acccounts - and
 * especially as an Account with any Entries or BudgetItems could
 * never be deleted due to database constraints - but it is worth bearing in
 * mind.
 */
class AccountListCtrl: public wxListCtrl
{
public:

	/**
	 * @returns a widget that lists all and only the Accounts stored
	 * in p_database_connection that are of AccountSuperType
	 * p_account_super_type. The Accounts are listed in order by
	 * AccountType and then by name. The widget displays, in two
	 * columns, the name and the balance (i.e. \e friendly_balance())
	 * of each Account.
	 */
	AccountListCtrl
	(	wxWindow* p_parent,
		PhatbooksDatabaseConnection& p_database_connection,
		AccountSuperType p_account_super_type
	);

	AccountListCtrl(AccountListCtrl const&) = delete;
	AccountListCtrl(AccountListCtrl&&) = delete;
	AccountListCtrl& operator=(AccountListCtrl const&) = delete;
	AccountListCtrl& operator=(AccountListCtrl&&) = delete;
	virtual ~AccountListCtrl();

	/**
	 * Populates \e out with the Ids of all and only the currently selected
	 * Accounts in the AccountListCtrl. If an Account in the list does not
	 * have an Id, then it will not be placed in \e out.
	 */
	void selected_accounts(std::set<sqloxx::Id>& out) const;

	/**
	 * Redraw AccountListCtrl on the basis of what is currently in the
	 * database, showing whichever AccountSuperType is currently being
	 * shown.
	 */
	void update();

	/**
	 * @returns an optional containing a handle to what may be
	 * considered the default Account in the AccountListCtrl; except that, if
	 * the AccountListCtrl is empty, returns an uninitialized optional.
	 */
	boost::optional<AccountHandle> default_account() const;

	/**
	 * Causes \e p_account to be selected, if it is present, and causes
	 * all other Accounts in the list to be deselected.
	 *
	 * Precondition: \e p_account must have an Id.
	 */
	void select_only(AccountHandle const& p_account);

	/**
	 * Toggle whether hidden Accounts are shown.
	 * 
	 * @returns \e true if and only if, after the function has been executed,
	 * hidden Accounts will be shown.
	 */
	bool toggle_showing_hidden();

private:

	void on_item_activated(wxListEvent& event);

	bool showing_daily_budget() const;

	bool m_show_hidden;
	AccountSuperType const m_account_super_type;
	PhatbooksDatabaseConnection& m_database_connection;

	static int const s_name_col = 0;
	static int const s_balance_col = s_name_col + 1;
	static int const s_budget_col = s_name_col + 2;

	DECLARE_EVENT_TABLE()
};


}  // namespace gui
}  // namespace phatbooks



#endif  // GUARD_account_list_ctrl_hpp_4892309124065707

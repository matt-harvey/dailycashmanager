/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_account_list_ctrl_hpp_4892309124065707
#define GUARD_account_list_ctrl_hpp_4892309124065707

#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include <sqloxx/handle_fwd.hpp>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/wx.h>
#include <set>

namespace dcm
{

// begin forward declarations

class Account;
class DcmDatabaseConnection;

// end forward declarations

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
		DcmDatabaseConnection& p_database_connection,
		AccountSuperType p_account_super_type
	);

	AccountListCtrl(AccountListCtrl const&) = delete;
	AccountListCtrl(AccountListCtrl&&) = delete;
	AccountListCtrl& operator=(AccountListCtrl const&) = delete;
	AccountListCtrl& operator=(AccountListCtrl&&) = delete;
	virtual ~AccountListCtrl();

	/**
	 * @returns a set of Ids of all and only the currently selected
	 * Accounts in the AccountListCtrl. If an Account in the list does not
	 * have an Id, then it will not be included in the returned set.
	 */
	std::set<sqloxx::Id> selected_accounts() const;

	/**
	 * Redraw AccountListCtrl on the basis of what is currently in the
	 * database, showing whichever AccountSuperType is currently being
	 * shown.
	 */
	void update();

	/**
	 * @returns a Handle to what may be
	 * considered the default Account in the AccountListCtrl; except that, if
	 * the AccountListCtrl is empty, returns a null Handle.
	 */
	sqloxx::Handle<Account> default_account() const;

	/**
	 * Causes \e p_account to be selected, if it is present, and causes
	 * all other Accounts in the list to be deselected.
	 *
	 * Precondition: \e p_account must have an Id.
	 */
	void select_only(sqloxx::Handle<Account> const& p_account);

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
	DcmDatabaseConnection& m_database_connection;

	static int const s_name_col = 0;
	static int const s_balance_col = s_name_col + 1;
	static int const s_budget_col = s_name_col + 2;

	DECLARE_EVENT_TABLE()
};


}  // namespace gui
}  // namespace dcm



#endif  // GUARD_account_list_ctrl_hpp_4892309124065707

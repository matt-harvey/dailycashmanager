// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_list_ctrl_hpp
#define GUARD_account_list_ctrl_hpp

#include "account.hpp"
#include "account_reader.hpp"
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
 */
class AccountListCtrl: public wxListCtrl
{
public:

	/**
	 * @returns a pointer to a heap-allocated AccountListCtrl, listing
	 * all and only the balance sheet accounts stored in \e dbc.
	 * The client does not need to take care of the memory - the memory
	 * is taken care of by the parent window.
	 *
	 * The name of each balance sheet Account is shown along with its balance.
	 */
	static AccountListCtrl* create_balance_sheet_account_list
	(	wxWindow* parent,
		PhatbooksDatabaseConnection& dbc
	);
		
	/**
	 * @returns a pointer to a heap-allocated AccountListCtrl, listing
	 * all and only the profit-and-loss accounts stored in \e dbc.
	 * The client does not need to take care of the memory - the memory
	 * is taken care of by the parent window.
	 *
	 * The name of each P&L Account is shown along with its envelope
	 * balance and daily budget allocation.
	 *
	 * @todo Implement display of daily budget allocation.
	 */
	static AccountListCtrl* create_pl_account_list
	(	wxWindow* parent,
		PhatbooksDatabaseConnection& dbc
	);

	/**
	 * Populates \e out with the Ids of all and only the currently selected
	 * Accounts in the AccountListCtrl. If an Account in the list does not
	 * have an Id, then it will not be placed in \e out.
	 */
	void selected_accounts(std::set<Account::Id>& out) const;

	/**
	 * Redraw AccountListCtrl on the basis of what is currently in the
	 * database, showing whichever AccountSuperType is currently being
	 * shown.
	 */
	void update();

	/**
	 * @returns an optional containing an Account that may be considered as
	 * the default Account in the AccountListCtrl; except that, if the
	 * AccountListCtrl is empty, returns an uninitialized optional.
	 */
	boost::optional<Account> default_account() const;

	/**
	 * Causes \e p_account to be selected, if it is present, and causes
	 * all other Accounts in the list to be deselected.
	 *
	 * Precondition: \e p_account must have an Id.
	 */
	void select_only(Account const& p_account);

	/**
	 * Toggle whether hidden Accounts are shown.
	 * 
	 * @returns \e true if and only if, after the function has been executed,
	 * hidden Accounts will be shown.
	 */
	bool toggle_showing_hidden();

private:

	/**
	 * Redraw AccountListCtrl on the basis of what is currently in the
	 * database, showing Accounts of \e p_account_super_type only.
	 *
	 * @todo Should be using inheritance here rather than this
	 * AccountSuperType flag.
	 */
	void update(account_super_type::AccountSuperType p_account_super_type);

	AccountListCtrl
	(	wxWindow* p_parent,
		AccountReaderBase const& p_reader,
		PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_left_column_title,
		account_super_type::AccountSuperType p_account_super_type
	);

	void on_item_activated(wxListEvent& event);

	void update
	(	AccountReaderBase const& p_reader,
		wxString const& p_left_column_title
	);

	bool showing_daily_budget() const;

	bool m_show_hidden;
	account_super_type::AccountSuperType m_account_super_type;
	PhatbooksDatabaseConnection& m_database_connection;

	static int const s_name_col = 0;
	static int const s_balance_col = s_name_col + 1;
	static int const s_budget_col = s_name_col + 2;

	DECLARE_EVENT_TABLE()
};


// INLINE FUNCTION IMPLEMENTATIONS

inline
void
AccountListCtrl::update()
{
	update(m_account_super_type);
	return;
}


}  // namespace gui
}  // namespace phatbooks



#endif  // GUARD_account_list_ctrl_hpp

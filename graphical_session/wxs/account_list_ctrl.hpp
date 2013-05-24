// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_list_ctrl_hpp
#define GUARD_account_list_ctrl_hpp

#include "account.hpp"
#include "account_reader.hpp"
#include <wx/listctrl.h>
#include <wx/wx.h>
#include <vector>

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
	 * Populates \e out with all and only the currently selected Accounts in
	 * the AccountListCtrl.
	 */
	void selected_accounts(std::vector<Account>& out);

	/**
	 * Redraw AccountListCtrl on the basis of what is currently in the
	 * database.
	 *
	 * @todo Should be using inheritance here rather than this boolean flag.
	 */
	void update(bool balance_sheet);


private:

	AccountListCtrl
	(	wxWindow* p_parent,
		AccountReaderBase const& p_reader,
		PhatbooksDatabaseConnection& p_database_connection,
		bool p_show_daily_budget
	);

	void update(AccountReaderBase const& p_reader);

	bool m_show_daily_budget;
	PhatbooksDatabaseConnection& m_database_connection;

	static int const s_name_col = 0;
	static int const s_balance_col = s_name_col + 1;
	static int const s_budget_col = s_name_col + 2;
};



}  // namespace gui
}  // namespace phatbooks



#endif  // GUARD_account_list_ctrl_hpp

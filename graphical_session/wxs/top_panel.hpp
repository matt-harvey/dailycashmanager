// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_top_panel_hpp
#define GUARD_top_panel_hpp

#include "account.hpp"
#include <wx/panel.h>
#include <wx/sizer.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class OrdinaryJournal;
class PhatbooksDatabaseConnection;

namespace gui
{

class AccountListCtrl;
class Frame;
class TransactionCtrl;

// End forward declarations


/**
 * Top level panel intended as immediate child of Frame.
 */
class TopPanel: public wxPanel
{
public:

	TopPanel
	(	Frame* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Populates \e out with a vector of the balance sheet Accounts currently
	 * selected by the user in the main window.
	 */
	void selected_balance_sheet_accounts(std::vector<Account>& out) const;

	/**
	 * Populates \e out with a vector of the P&L Accounts currently selected
	 * by the user in the main window.
	 */
	void selected_pl_accounts(std::vector<Account>& out) const;

	/**
	 * Update the display to reflect current state of database.
	 */
	void update();

	/**
	 * Configure the TransactionCtrl to reflect the Accounts passed in the
	 * parameters.
	 *
	 * @param p_balance_sheet_accounts a possibly empty sequence of Accounts
	 * of account_super_type::balance_sheet.
	 *
	 * @param p_pl_accounts a possibly empty sequence of Accounts of
	 * account_super_type::pl.
	 */
	void configure_transaction_ctrl
	(	std::vector<Account> p_balance_sheet_accounts,
		std::vector<Account> p_pl_accounts
	);

private:

	void configure_account_lists();

	/**
	 * Configure the TransactionCtrl to reflect the currently selected
	 * Accounts (if any).
	 *
	 * @todo What if fewer than 2 Accounts are selected?
	 */
	void configure_transaction_ctrl();

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	AccountListCtrl* m_bs_account_list;
	AccountListCtrl* m_pl_account_list;
	TransactionCtrl* m_transaction_ctrl;
};



}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_top_panel_hpp

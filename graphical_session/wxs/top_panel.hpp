// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_top_panel_hpp
#define GUARD_top_panel_hpp

#include "account.hpp"
#include <wx/wx.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class AccountListCtrl;
class EntryListCtrl;
class Frame;

// End forward declarations


/**
 * Top level panel intended as immediate child of Frame.
 */
class TopPanel:
	public wxPanel
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

private:

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	AccountListCtrl* m_bs_account_list;
	AccountListCtrl* m_pl_account_list;
	EntryListCtrl* m_entry_list;
};



}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_top_panel_hpp

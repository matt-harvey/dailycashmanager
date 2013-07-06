// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_top_panel_hpp
#define GUARD_top_panel_hpp

#include "account.hpp"
#include <wx/notebook.h>
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
class DraftJournalListCtrl;
class EntryListCtrl;
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
	 * Update the display to reflect current state of database, after
	 * saving of p_saved_object.
	 */
	void update_for(DraftJournal const& p_saved_object);
	void update_for(OrdinaryJournal const& p_saved_object);
	void update_for(Account const& p_saved_object);

	/**
	 * Configure the TransactionCtrl to reflect the currently selected
	 * Accounts (if any).
	 *
	 * @todo What if fewer than 2 Accounts are selected?
	 */
	void configure_transaction_ctrl();

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

	void configure_draft_journal_list_ctrl();

private:

	void configure_account_lists();

	void configure_entry_list();

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	wxNotebook* m_notebook;
	wxPanel* m_notebook_page_1;
	wxPanel* m_notebook_page_2;
	wxBoxSizer* m_right_column_sizer;
	AccountListCtrl* m_bs_account_list;
	AccountListCtrl* m_pl_account_list;
	EntryListCtrl* m_entry_list;
	TransactionCtrl* m_transaction_ctrl;
	DraftJournalListCtrl* m_draft_journal_list;
};



}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_top_panel_hpp

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_ctrl_hpp
#define GUARD_transaction_ctrl_hpp

#include "account.hpp"
#include "decimal_text_ctrl.hpp"
#include "transaction_type_ctrl.hpp"
#include <jewel/on_windows.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class DateCtrl;
class TopPanel;

// End forward declarations




/**
 * Dialog in which user creates a new transaction (i.e. Journal).
 *
 * @todo Ensure Journal balances.
 *
 * @todo Provide option to save as Draft, Recurring or etc. etc..
 *
 * @todo When the update the TopPanel after posting or cancelling Journal
 * from TransactionCtrl, there is flicker. This is because we are redrawing
 * the entire TopPanel from scratch. We could probably get rid of the flicker
 * by only redrawing the parts we need.
 *
 * @todo Ensure this can handle situation where insufficient default
 * Accounts are available.
 */
class TransactionCtrl: public wxPanel
{
public:
	
	/**
	 * All Accounts in p_accounts should be associated with the same
	 * PhatbooksDatabaseConnection.
	 *
	 * @param p_parent parent panel
	 *
	 * @param p_balance_sheet_accounts a sequence of Accounts
	 * all of which must be balance sheet Accounts (i.e. of
	 * account_super_type::balance_sheet). May be empty providing
	 * p_pl_accounts has at least 2 elements.
	 *
	 * @param p_pl_accounts a sequence of Accounts all of which
	 * must be P&L Accounts (i.e. of account_super_type::pl). May be empty
	 * providing p_balance_sheet_accounts has at least 2 elements.
	 *
	 * @param p_database_connection database connection. Must be the same
	 * one that the Accounts in p_balance_sheet_accounts and
	 * p_pl_accounts are associated with.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		std::vector<Account> const& p_balance_sheet_accounts,
		std::vector<Account> const& p_pl_accounts,
		PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Refresh the selections available in the AccountCtrls in the
	 * TransactionCtrl, and the selection in the TransactionTypeCtrl,
	 * to match p_transaction_type.
	 */
	void refresh_for_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);
	
protected:

private:
	void on_ok_button_click(wxCommandEvent& event);
	void on_recurring_transaction_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);
	void post_journal() const;
	bool is_balanced() const;

	int m_max_entry_row_id;

	wxGridBagSizer* m_top_sizer;

	// Re. "source side" of transaction
	wxStaticText* m_source_side_phrase;

	// Re. "destinations side" of transaction
	wxStaticText* m_destination_side_phrase;

	TransactionTypeCtrl* m_transaction_type_ctrl;
	DecimalTextCtrl* m_primary_amount_ctrl;
	DateCtrl* m_date_ctrl;
	
	wxButton* m_cancel_button;
	wxButton* m_recurring_transaction_button;
	wxButton* m_ok_button;

	std::vector<AccountCtrl*> m_account_name_boxes;
	std::vector<wxTextCtrl*> m_comment_boxes;
	std::vector<wxButton*> m_split_buttons;

	static unsigned int const s_date_ctrl_id =
		wxID_HIGHEST + 1;
	static unsigned int const s_primary_amount_ctrl_id =
		s_date_ctrl_id + 1;
	static unsigned int const s_recurring_transaction_button_id =
		s_primary_amount_ctrl_id + 1;	
	static unsigned int const s_min_entry_row_id =
		s_recurring_transaction_button_id + 1;

	DECLARE_EVENT_TABLE()

	PhatbooksDatabaseConnection& m_database_connection;

};  // class TransactionCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_ctrl_hpp

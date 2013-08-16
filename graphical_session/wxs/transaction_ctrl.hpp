// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_ctrl_hpp
#define GUARD_transaction_ctrl_hpp

#include "account.hpp"
#include "decimal_text_ctrl.hpp"
#include "gridded_scrolled_panel.hpp"
#include "transaction_type_ctrl.hpp"
#include "transaction_type.hpp"
#include <jewel/decimal_fwd.hpp>
#include <jewel/on_windows.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

namespace phatbooks
{

// Begin forward declarations

class DraftJournal;
class OrdinaryJournal;
class PersistentJournal;
class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class DateCtrl;
class EntryGroupCtrl;
class FrequencyCtrl;
class TopPanel;

// End forward declarations


/**
 * Dialog in which user creates a new transaction (i.e. Journal).
 *
 * @todo Ensure this can handle situation where insufficient default
 * Accounts are available.
 *
 * @todo When editing an existing transaction, if the user changes the
 * TransactionType, then the AccountCtrls are reset, and the user
 * loses the existing Account information. Prevent this from occurring.
 *
 * @todo When user deletes a Journal where some or all of the Entries
 * in that Journal have already been reconciled, then we need either
 * to prevent this from happening at all, or else to warn the user
 * that the reconciliations will be out of whack.
 *
 * @todo When user creates or amended an Account, the TransactionCtrl should
 * be updated seamlessly, rather than being recreated from scratch.
 *
 * @todo It's not obvious to the user that the TransactionCtrl actually is
 * for creating new transactions (rather than for, say, filtering the
 * information that is displayed to the left).
 */
class TransactionCtrl: public GriddedScrolledPanel
{
public:
	
	/**
	 * All Accounts in p_accounts should be associated with the same
	 * PhatbooksDatabaseConnection.
	 *
	 * Note, with the vectors of Accounts passed to p_balance_sheet_accounts
	 * and p_pl_accounts, not every Account in these vectors will necessarily
	 * be used. If both vectors are non-empty then only the 0th Account
	 * in each vector will be used. If one of the vectors is empty,
	 * then the front 2 Accounts in the other vector will be used, and that's
	 * all.
	 *
	 * @todo This constructor is entirely non-obvious in the way it
	 * behaves. Instead of passing two vectors, we would expect to pass two
	 * Accounts, one the source Account and the other the destination
	 * Account.
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
	 * 
	 * @todo HIGH PRIORITY Boundary around comment boxes and date box looks
	 * wrong under certain circumstances under Windows.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		wxSize const& p_size,
		std::vector<Account> const& p_balance_sheet_accounts,
		std::vector<Account> const& p_pl_accounts,
		PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Create a TransactionCtrl to allow the user to edit an
	 * already-saved OrdinaryJournal.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		wxSize const& p_size,
		OrdinaryJournal const& p_journal
	);

	/**
	 * Create a TransactionCtrl to allow the user to edit an
	 * already-saved DraftJournal.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		wxSize const& p_size,
		DraftJournal const& p_journal
	);

	~TransactionCtrl();

	/**
	 * Refresh the selections available in the AccountCtrls in the
	 * TransactionCtrl, and the selection in the TransactionTypeCtrl,
	 * to match p_transaction_type.
	 */
	void refresh_for_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);

	jewel::Decimal primary_amount() const;
	
private:
	void on_cancel_button_click(wxCommandEvent& event);
	void on_delete_button_click(wxCommandEvent& event);
	void on_ok_button_click(wxCommandEvent& event);

	/*
	void disable_editing();
	void enable_editing(bool p_enable = true);
	*/

	// Adds some blank space to the right to allow space for vertical
	// scrollbar.
	void add_dummy_column();

	// Places size of "standard text box" in p_text_box_size.
	void configure_top_controls
	(	transaction_type::TransactionType p_transaction_type,
		wxSize& p_text_box_size,
		jewel::Decimal const& p_primary_amount,
		std::vector<transaction_type::TransactionType> const&
			p_available_transaction_types
	);

	void configure_for_journal_editing();

	/**
	 * @returns true if and only if journal was actually posted.
	 */
	bool post_journal();

	/**
	 * Removes *m_journal from database.
	 *
	 * @returns true if and only if journal was actually removed.
	 */
	bool remove_journal();

	/**
	 * @returns true if and only if existing journal was successfully saved.
	 */
	bool save_existing_journal();

	bool is_balanced() const;

	int m_max_entry_row_id;

	TransactionTypeCtrl* m_transaction_type_ctrl;

	EntryGroupCtrl* m_source_entry_ctrl;	
	EntryGroupCtrl* m_destination_entry_ctrl;

	DecimalTextCtrl* m_primary_amount_ctrl;
	FrequencyCtrl* m_frequency_ctrl;
	DateCtrl* m_date_ctrl;
	
	wxButton* m_cancel_button;
	wxButton* m_delete_button;
	wxButton* m_ok_button;

	static unsigned int const s_date_ctrl_id =
		wxID_HIGHEST + 1;
	static unsigned int const s_primary_amount_ctrl_id =
		s_date_ctrl_id + 1;
	static unsigned int const s_transaction_type_ctrl_id =
		s_primary_amount_ctrl_id + 1;
	static unsigned int const s_delete_button_id =
		s_transaction_type_ctrl_id + 1;
	static unsigned int const s_min_entry_row_id =
		s_delete_button_id + 1;

	PersistentJournal* m_journal;

	DECLARE_EVENT_TABLE()

};  // class TransactionCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_ctrl_hpp

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_ctrl_hpp
#define GUARD_transaction_ctrl_hpp

#include "account.hpp"
#include "decimal_text_ctrl.hpp"
#include "transaction_type_ctrl.hpp"
#include "transaction_type.hpp"
#include <boost/noncopyable.hpp>
#include <jewel/decimal_fwd.hpp>
#include <jewel/on_windows.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/gbsizer.h>
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
class EntryCtrl;
class FrequencyCtrl;
class TopPanel;

// End forward declarations


/**
 * Dialog in which user creates a new transaction (i.e. Journal).
 *
 * @todo When the update the TopPanel after posting or cancelling Journal
 * from TransactionCtrl, there is flicker. This is because we are redrawing
 * the entire TopPanel from scratch. We could probably get rid of the flicker
 * by only redrawing the parts we need.
 *
 * @todo Ensure this can handle situation where insufficient default
 * Accounts are available.
 *
 * @todo Make it so that it is easier for the user to balance the Journal.
 * Things that could facilitate this include enabling simple arithmetic,
 * showing the amount of imbalance somewhere in the TransactionCtrl, and
 * perhaps automatically putting the imbalanced amount in the last Entry
 * (although that might result in surprising behaviour for this user?).
 *
 * @todo This needs to be able to scroll.
 *
 * @todo This crashes when we save a DraftJournal.
 */
class TransactionCtrl: public wxPanel, private boost::noncopyable
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
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		std::vector<Account> const& p_balance_sheet_accounts,
		std::vector<Account> const& p_pl_accounts,
		PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Create a TransactionCtrl to allow the user to edit an
	 * already-saved OrdinaryJournal.
	 */
	TransactionCtrl(TopPanel* p_parent, OrdinaryJournal const& p_journal);

	/**
	 * Create a TransactionCtrl to allow the user to edit an
	 * already-saved DraftJournal.
	 */
	TransactionCtrl(TopPanel* p_parent, DraftJournal const& p_journal);

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
	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	// Returns next row.
	// Places size of "standard text box" in p_text_box_size.
	size_t configure_top_controls
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
	 * @returns true if and only if existing journal was successfully saved.
	 */
	bool save_existing_journal();

	bool is_balanced() const;

	int m_max_entry_row_id;

	wxGridBagSizer* m_top_sizer;

	TransactionTypeCtrl* m_transaction_type_ctrl;

	EntryCtrl* m_source_entry_ctrl;	
	EntryCtrl* m_destination_entry_ctrl;

	DecimalTextCtrl* m_primary_amount_ctrl;
	FrequencyCtrl* m_frequency_ctrl;
	DateCtrl* m_date_ctrl;
	
	wxButton* m_cancel_button;
	wxButton* m_ok_button;

	static unsigned int const s_date_ctrl_id =
		wxID_HIGHEST + 1;
	static unsigned int const s_primary_amount_ctrl_id =
		s_date_ctrl_id + 1;
	static unsigned int const s_transaction_type_ctrl_id =
		s_primary_amount_ctrl_id + 1;
	static unsigned int const s_min_entry_row_id =
		s_transaction_type_ctrl_id + 1;

	PersistentJournal* m_journal;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class TransactionCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_ctrl_hpp

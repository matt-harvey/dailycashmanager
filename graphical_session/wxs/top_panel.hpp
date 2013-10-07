// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_top_panel_hpp_7915960996372607
#define GUARD_top_panel_hpp_7915960996372607

#include "account_type.hpp"
#include "draft_journal_handle_fwd.hpp"
#include "entry_handle.hpp"
#include "ordinary_journal_handle.hpp"
#include "reconciliation_list_panel.hpp"
#include "sizing.hpp"
#include "transaction_ctrl.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class PhatbooksDatabaseConnection;
class ProtoJournal;

namespace gui
{

class AccountListCtrl;
class DraftJournalListCtrl;
class EntryListPanel;
class Frame;
class ReportPanel;
class TransactionCtrl;

// End forward declarations


/**
 * Top level panel intended as immediate child of Frame.
 *
 * @todo The various "update_for_..." functions each contain calls to
 * analogous "update_for_..." functions for each of the sub-widgets
 * in TopPanel. This makes for repetitive code. We could probably streamline
 * this and make it more maintainable either by using wxWidgets' event
 * system, or else by having an abstract base class for the various
 * sub-widget classes, and then storing pointers to these in a vector
 * in TopPanel, and running through the vector calling "update_for_..." on
 * each.
 *
 * @todo Put some space between the two AccountListCtrls, or do some other
 * thing, to clearly indicate that these are conceptually separate controls,
 * and that one is not, say, a subset of the other. Put a similar kind of
 * "divider" between the envelope list and the TransactionCtrl to the right.
 */
class TopPanel: public wxPanel
{
public:

	TopPanel
	(	Frame* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

	TopPanel(TopPanel const&) = delete;
	TopPanel(TopPanel&&) = delete;
	TopPanel& operator=(TopPanel const&) = delete;
	TopPanel& operator=(TopPanel&&) = delete;
	~TopPanel() = default;

	/**
	 * Populates \e out with handles to all the balance sheet Accounts currently
	 * selected by the user in the main window.
	 */
	void selected_balance_sheet_accounts
	(	std::vector<sqloxx::Handle<Account> >& out
	) const;

	/**
	 * Populates \e out with handles to all the P&L Accounts currently selected
	 * by the user in the main window.
	 */
	void selected_pl_accounts(std::vector<sqloxx::Handle<Account> >& out) const;

	/**
	 * Populates \e out with all the OrdinaryJournals currently
	 * selected by the user in the main window.
	 */
	void selected_ordinary_journals(std::vector<OrdinaryJournalHandle>& out) const;

	/**
	 * Populates \e out with all the DraftJournals currently
	 * selected by the user in the main window.
	 */
	void selected_draft_journals(std::vector<DraftJournalHandle>& out) const;

	/**
	 * Update the display to reflect current state of database, after
	 * saving of p_saved_object, where p_saved_object is a newly saved
	 * object that was not already in the database.
	 */
	void update_for_new(DraftJournalHandle const& p_saved_object);
	void update_for_new(OrdinaryJournalHandle const& p_saved_object);
	void update_for_new(sqloxx::Handle<Account> const& p_saved_object);

	/**
	 * Update the display to reflect current state of database, after
	 * saving of p_saved_object, where p_saved_object already existed
	 * in the database, but has just been amended and the amendments
	 * saved.
	 *
	 * @todo These assume no Entries have been deleted from the
	 * Journal in question. This is assumption is correct only as long
	 * as TransactionCtrl does not support deletion of individual
	 * Entries.
	 */
	void update_for_amended(DraftJournalHandle const& p_saved_object);
	void update_for_amended(OrdinaryJournalHandle const& p_saved_object);
	void update_for_amended(sqloxx::Handle<Account> const& p_saved_object);

	/**
	 * Update the display to reflect that the object with
	 * p_doomed_id (or the objects with p_doomed_ids) have just
	 * been removed from the database.
	 */
	void update_for_deleted_ordinary_journal(sqloxx::Id p_doomed_id);
	void update_for_deleted_draft_journal(sqloxx::Id p_doomed_id);
	void update_for_deleted_ordinary_entries
	(	std::vector<sqloxx::Id> const& p_doomed_ids
	);
	void update_for_deleted_draft_entries
	(	std::vector<sqloxx::Id> const& p_doomed_ids
	);

	/**
	 * Update the display to reflect current state of database, after
	 * BudgetItems pertaining to p_account have been added, deleted
	 * and/or amended.
	 * 
	 * Precondition: p_account should be of AccountSuperType::pl.
	 */
	void update_for_amended_budget(sqloxx::Handle<Account> const& p_account);

	/**
	 * Update the display to reflect the current state of \e p_entry
	 * with respect just to whether it is reconciled.
	 *
	 * WARNING This is messy and "coupled", but: this intentionally does
	 * \e not update the
	 * ReconciliationListPanel / ReconciliationEntryListCtrl, as it
	 * is assumed these are the \e source of the change - we don't update
	 * these \e again, on pain of circularity.
	 */
	void update_for_reconciliation_status(EntryHandle const& p_entry);

	/**
	 * @returns a ProtoJournal containing two Entries, with blank
	 * comments, and with Accounts based either on the
	 * most commonly used Accounts in the database, or on Accounts
	 * currently selected by the user in the EntryListCtrls.
	 */
	ProtoJournal make_proto_journal() const;

	/**
	 * Configure the TransactionCtrl to reflect the currently selected
	 * Accounts (if any).
	 *
	 * @todo What if fewer than 2 Accounts are selected?
	 */
	void configure_transaction_ctrl();

	/**
	 * Configure the TransactionCtrl to reflect an existing OrdinaryJournal
	 * or DraftJournal.
	 * \e JournalType should be either OrdinaryJournal or DraftJournal.
	 */
	template <typename JournalType>
	void configure_transaction_ctrl(JournalType& p_journal);

	void configure_draft_journal_list_ctrl();

	/**
	 * @returns \e true if and only if we will now be showing
	 * hidden Accounts of AccountSuperType \e p_account_super_type.
	 */
	bool toggle_show_hidden_accounts
	(	AccountSuperType p_account_super_type
	);

private:

	void configure_account_lists();
	void configure_entry_list();
	void configure_reconciliation_page();
	void configure_report_page();

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	wxNotebook* m_notebook;
	wxPanel* m_notebook_page_accounts;
	wxPanel* m_notebook_page_transactions;
	wxPanel* m_notebook_page_reconciliations;
	wxPanel* m_notebook_page_reports;
	wxBoxSizer* m_right_column_sizer;
	AccountListCtrl* m_bs_account_list;
	AccountListCtrl* m_pl_account_list;
	EntryListPanel* m_entry_list_panel;
	ReconciliationListPanel* m_reconciliation_panel;
	ReportPanel* m_report_panel;
	TransactionCtrl* m_transaction_ctrl;
	DraftJournalListCtrl* m_draft_journal_list;
};



// IMPLEMENT MEMBER FUNCTION TEMPLATE

template <typename JournalType>
void
TopPanel::configure_transaction_ctrl(JournalType& p_journal)
{
	TransactionCtrl* old = nullptr;
	JEWEL_ASSERT (m_right_column_sizer);
	if (m_transaction_ctrl)
	{
		m_right_column_sizer->Detach(m_transaction_ctrl);
		old = m_transaction_ctrl;
	}
	m_transaction_ctrl = new TransactionCtrl
	(	this,
		wxSize(GetClientSize().x, 10000),
		p_journal
	);
	m_right_column_sizer->Insert
	(	0,
		m_transaction_ctrl,
		wxSizerFlags(6).Expand().
			Border(wxNORTH | wxSOUTH | wxWEST | wxEAST, standard_border() * 2)
	);
	if (old)
	{
		old->Destroy();
		old = nullptr;
	}
	Layout();
	JEWEL_ASSERT (m_transaction_ctrl);
	// m_transaction_ctrl->SetFocus();  // This doesn't seem to have any effect
	return;
}


}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_top_panel_hpp_7915960996372607

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_frame_hpp
#define GUARD_frame_hpp

#include "account.hpp"
#include "draft_journal.hpp"
#include "ordinary_journal.hpp"
#include <wx/menu.h>
#include <wx/wx.h>
#include <wx/string.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class TopPanel;

// End forward declarations

class Frame: public wxFrame
{
public:

	Frame
	(	wxString const& title,
		PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Populates \e out with all the balance sheet Accounts currently
	 * selected by the user in the main window.
	 */
	void selected_balance_sheet_accounts(std::vector<Account>& out) const;

	/**
	 * Populates \e out with all the P&L Accounts currently selected
	 * by the user in the main window.
	 */
	void selected_pl_accounts(std::vector<Account>& out) const;

	/**
	 * Populates \e out with all the OrdinaryJournals currently
	 * selected by the user in the main window.
	 */
	void selected_ordinary_journals(std::vector<OrdinaryJournal>& out) const;

	/**
	 * Populates \e out with all the DraftJournals currently
	 * selected by the user in the main window.
	 */
	void selected_draft_journals(std::vector<DraftJournal>& out) const;

private:

	// Event handlers
	void on_quit(wxCommandEvent& event);
	void on_about(wxCommandEvent& event);
	void on_new_bs_account(wxCommandEvent& event);
	void on_new_pl_account(wxCommandEvent& event);
	void on_new_transaction(wxCommandEvent& event);
	void on_edit_bs_account(wxCommandEvent& event);
	void on_edit_pl_account(wxCommandEvent& event);
	void on_edit_ordinary_journal(wxCommandEvent& event);
	void on_edit_draft_journal(wxCommandEvent& event);
	void on_edit_budget(wxCommandEvent& event);

	static int const s_new_bs_account_id = wxID_HIGHEST + 1;
	static int const s_new_pl_account_id = s_new_bs_account_id + 1;
	static int const s_new_transaction_id = s_new_pl_account_id + 1;
	static int const s_edit_bs_account_id = s_new_transaction_id + 1;
	static int const s_edit_pl_account_id = s_edit_bs_account_id + 1;
	static int const s_edit_ordinary_journal_id = s_edit_pl_account_id + 1;
	static int const s_edit_draft_journal_id = s_edit_ordinary_journal_id + 1;
	static int const s_edit_budget_id = s_edit_draft_journal_id + 1;

	PhatbooksDatabaseConnection& m_database_connection;

	wxMenuBar* m_menu_bar;
	wxMenu* m_file_menu;
	wxMenu* m_new_menu;
	wxMenu* m_edit_menu;
	wxMenu* m_help_menu;

	TopPanel* m_top_panel;
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_frame_hpp
#define GUARD_frame_hpp

#include "account.hpp"
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

	// Event handlers
	void on_quit(wxCommandEvent& event);
	void on_about(wxCommandEvent& event);
	void on_new_bs_account(wxCommandEvent& event);
	void on_new_pl_account(wxCommandEvent& event);
	void on_new_transaction(wxCommandEvent& event);
	void on_edit_bs_account(wxCommandEvent& event);
	void on_edit_pl_account(wxCommandEvent& event);

	static int const s_new_bs_account_id = wxID_HIGHEST + 1;
	static int const s_new_pl_account_id = s_new_bs_account_id + 1;
	static int const s_new_transaction_id = s_new_pl_account_id + 1;
	static int const s_edit_bs_account_id = s_new_transaction_id + 1;
	static int const s_edit_pl_account_id = s_edit_bs_account_id + 1;

	PhatbooksDatabaseConnection& m_database_connection;
	TopPanel* m_top_panel;

	wxMenuBar* m_menu_bar;
	wxMenu* m_file_menu;
	wxMenu* m_new_menu;
	wxMenu* m_edit_menu;
	wxMenu* m_help_menu;
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp

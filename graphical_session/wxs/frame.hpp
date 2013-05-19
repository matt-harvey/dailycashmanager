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

	// Event handlers
	void on_quit(wxCommandEvent& event);
	void on_about(wxCommandEvent& event);
	void on_test_selected_accounts(wxCommandEvent& event);

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
	TopPanel* m_top_panel;

	wxMenuBar* m_menu_bar;
	wxMenu* m_file_menu;
	wxMenu* m_test_menu;
	wxMenu* m_help_menu;
	
	static int const s_test_selected_accounts_id = wxID_HIGHEST + 1;
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp

#include "frame.hpp"
#include "account.hpp"
#include "account_list_ctrl.hpp"
#include "app.hpp"
#include "entry_list_ctrl.hpp"
#include "icon.xpm"
#include "phatbooks_database_connection.hpp"
#include "top_panel.hpp"
#include <jewel/on_windows.hpp>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/wx.h>
#include <vector>

using std::vector;

#include <iostream>  // for testing
using std::cout;     // for testing
using std::endl;     // for testing


namespace phatbooks
{
namespace gui
{


Frame::Frame
(	wxString const& title,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxFrame
	(	0,
		wxID_ANY,
		title,
		wxDefaultPosition,
#		if JEWEL_ON_WINDOWS
			wxDefaultSize
#		else
			wxSize
			(	wxSystemSettings::GetMetric(wxSYS_SCREEN_X),
				wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)
			)
#		endif
	),
	m_database_connection(p_database_connection),
	m_top_panel(0),
	m_menu_bar(0),
	m_file_menu(0),
	m_test_menu(0),
	m_help_menu(0)
{
	// Set the frame icon
	// TODO I should use SetIcons to associate several icons of
	// different sizes with the Window. This avoids possible ugliness from
	// resizing of a single icon when different sizes are required in
	// different situations.
	SetIcon(wxIcon(icon_xpm));

	// Create menus
	m_menu_bar = new wxMenuBar;
	m_file_menu = new wxMenu;
	m_test_menu = new wxMenu;
	m_help_menu = new wxMenu;
	m_file_menu->Append
	(	wxID_EXIT,
		wxT("E&xit\tAlt-X"),
		wxT("Quit this program")
	);
	m_test_menu->Append
	(	s_test_selected_accounts_id,
		wxT("&Selected accounts"),
		wxT("Print names of selected Accounts to console")
	);
	m_help_menu->Append
	(	wxID_ABOUT,
		wxT("&About...\tF1"),
		wxT("Show about dialog")
	);
	m_menu_bar->Append(m_file_menu, wxT("&File"));
	m_menu_bar->Append(m_test_menu, wxT("&Test"));
	m_menu_bar->Append(m_help_menu, wxT("&Help"));

	SetMenuBar(m_menu_bar);

	Connect
	(	wxID_EXIT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_quit)
	);
	Connect
	(	s_test_selected_accounts_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_test_selected_accounts)
	);
	Connect
	(	wxID_ABOUT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_about)
	);

#	if JEWEL_ON_WINDOWS
		Maximize();
#	endif

	m_top_panel = new TopPanel(this, m_database_connection);

}
	

void
Frame::on_about(wxCommandEvent& event)
{
	// TODO Put proper messages here
	wxString msg;
	msg.Printf("Hello and welcome to %s", wxVERSION_STRING);
	wxMessageBox(msg, "About Minimal", wxOK | wxICON_INFORMATION, this);
	(void)event;  // Silence compiler warning re. unused parameter.
	return;
}

void
Frame::on_quit(wxCommandEvent& event)
{
	// Destroy the frame
	Close();
	(void)event;  // Silence compiler warning re. unused parameter.
	return;
}

void
Frame::on_test_selected_accounts(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<Account> selected_accounts;
	selected_balance_sheet_accounts(selected_accounts);
	selected_pl_accounts(selected_accounts);
	for (vector<Account>::size_type i = 0; i != selected_accounts.size(); ++i)
	{
		cout << selected_accounts[i].name() << endl;
	}
	return;
}

void
Frame::selected_balance_sheet_accounts(vector<Account>& out) const
{
	m_top_panel->selected_balance_sheet_accounts(out);
	return;
}

void
Frame::selected_pl_accounts(vector<Account>& out) const
{
	m_top_panel->selected_pl_accounts(out);
	return;
}


}  // namespace gui
}  // namespace phatbooks

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

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

#include <jewel/debug_log.hpp>  // for debugging / testing
#include <iostream>  // for debugging / testinga
using std::cout;     // for debugging / testing
using std::endl;     // for debubbing / testing


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
	m_new_menu(0),
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
	m_new_menu = new wxMenu;
	m_help_menu = new wxMenu;
	m_file_menu->Append
	(	wxID_EXIT,
		wxT("E&xit\tAlt-X"),
		wxT("Quit this program")
	);
	m_new_menu->Append
	(	s_new_transaction_id,
		wxT("New &transaction \tAlt-T"),
		wxT("Record a new transaction")
	);
	m_help_menu->Append
	(	wxID_ABOUT,
		wxT("&About...\tF1"),
		wxT("Show about dialog")
	);
	m_menu_bar->Append(m_file_menu, wxT("&File"));
	m_menu_bar->Append(m_new_menu, wxT("&New"));
	m_menu_bar->Append(m_help_menu, wxT("&Help"));

	SetMenuBar(m_menu_bar);

	Connect
	(	wxID_EXIT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_quit)
	);
	Connect
	(	s_new_transaction_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_new_transaction)
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
Frame::on_new_transaction(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<Account> balance_sheet_accounts;
	selected_balance_sheet_accounts(balance_sheet_accounts);
	vector<Account> pl_accounts;
	selected_pl_accounts(pl_accounts);
	m_top_panel->
		configure_transaction_ctrl(balance_sheet_accounts, pl_accounts);
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

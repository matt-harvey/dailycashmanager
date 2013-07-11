// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "frame.hpp"
#include "account.hpp"
#include "account_dialog.hpp"
#include "account_list_ctrl.hpp"
#include "application.hpp"
#include "app.hpp"
#include "entry_list_ctrl.hpp"
#include "icon.xpm"
#include "ordinary_journal.hpp"
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
#include <iostream>  // for debugging / testing
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
	m_menu_bar(0),
	m_file_menu(0),
	m_new_menu(0),
	m_edit_menu(0),
	m_help_menu(0),
	m_top_panel(0)
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
	m_edit_menu = new wxMenu;
	m_help_menu = new wxMenu;

	// Configure "file" menu
	m_file_menu->Append
	(	wxID_EXIT,
		wxString("E&xit\tAlt-X"),
		wxString("Quit this program")
	);
	m_menu_bar->Append(m_file_menu, wxString("&File"));

	// Configure "new" menu
	m_new_menu->Append
	(	s_new_bs_account_id,
		wxString("New &account"),
		wxString("Create a new asset or liability account")
	);
	m_new_menu->Append
	(	s_new_pl_account_id,
		wxString("New &category"),
		wxString("Create a new revenue or expenditure category")
	);
	m_new_menu->Append
	(	s_new_transaction_id,
		wxString("New &transaction \tAlt-T"),
		wxString("Record a new transaction")
	);
	m_menu_bar->Append(m_new_menu, wxString("&New"));

	// Configure "edit" menu
	m_edit_menu->Append
	(	s_edit_bs_account_id,
		wxString("Edit selected &account"),
		wxString("Edit an exising asset or liability account")
	);
	m_edit_menu->Append
	(	s_edit_pl_account_id,
		wxString("Edit selected &category"),
		wxString("Edit an existing revenue or expenditure category")
	);
	m_edit_menu->Append
	(	s_edit_journal_id,
		wxString("Edit selected &transaction"),
		wxString("Edit an existing transaction")
	);
	m_menu_bar->Append(m_edit_menu, wxString("&Edit"));

	// Configure "help" menu
	m_help_menu->Append
	(	wxID_ABOUT,
		wxString("&About...\tF1"),
		wxString("Show about dialog")
	);
	m_menu_bar->Append(m_help_menu, wxString("&Help"));

	SetMenuBar(m_menu_bar);

	// Connect events to menus
	Connect
	(	wxID_EXIT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_quit)
	);
	Connect
	(	s_new_bs_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_new_bs_account)
	);
	Connect
	(	s_new_pl_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_new_pl_account)
	);
	Connect
	(	s_new_transaction_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_new_transaction)
	);
	Connect
	(	s_edit_bs_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_edit_bs_account)
	);
	Connect
	(	s_edit_pl_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_edit_pl_account)
	);
	Connect
	(	s_edit_journal_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_edit_ordinary_journal)
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
	(void)event;  // Silence compiler warning re. unused parameter.
	// TODO Put better message here
	wxString msg;
	msg.Printf
	(	"Hello and welcome to %s",
		bstring_to_wx(Application::application_name())
	);
	wxMessageBox(msg);
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
Frame::on_new_bs_account(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	Account account(m_database_connection);
	AccountDialog account_dialog
	(	this,
		account,
		account_super_type::balance_sheet
	);
	if (account_dialog.ShowModal() == wxID_OK)
	{
		// TODO This will obliterate any contents of the TransactionCtrl.
		// Do we want this? We probably \e do want it to update the
		// AccountTypeCtrl and AccountCtrls in the TransactionCtrl; but
		// we don't really want it to obliterate everything else.
		m_top_panel->update_for(account);
	}
	return;
}

void
Frame::on_new_pl_account(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	Account account(m_database_connection);
	AccountDialog account_dialog(this, account, account_super_type::pl);
	if (account_dialog.ShowModal() == wxID_OK)
	{
		// TODO This will obliterate any contents of the TransactionCtrl.
		// Do we want this? We probably \e do want it to update the
		// AccountTypeCtrl and AccountCtrls in the TransactionCtrl; but
		// we don't really want it to obliterate everything else.
		m_top_panel->update_for(account);
	}
	return;
}

void
Frame::on_edit_bs_account(wxCommandEvent& event)
{
	// TODO Factor out repeated code in the various member functions
	// that invoke an AccountDialog.

	(void)event;  // Silence compiler re. unused parameter.
	vector<Account> accounts;
	selected_balance_sheet_accounts(accounts);
	if (accounts.empty())
	{
		// TODO HIGH PRIORITY Deal with this in a more user-friendly
		// way. For now, we just do this.
		wxMessageBox
		(	"Account to edit must first be selected in main window."
		);
		return;
	}
	assert (accounts.size() >= 1);
	Account account = accounts[0];
	assert
	(	super_type(account.account_type()) ==
		account_super_type::balance_sheet
	);
	AccountDialog account_dialog
	(	this,
		account,
		account_super_type::balance_sheet
	);
	if (account_dialog.ShowModal() == wxID_OK)
	{
		// TODO This will obliterate any contents of the TransactionCtrl.
		// Do we want this? We probably \e do want it to update the
		// AccountTypeCtrl and AccountCtrls in the TransactionCtrl; but
		// we don't really want it to obliterate everything else.
		m_top_panel->update_for(account);
	}
	return;
}

void
Frame::on_edit_pl_account(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	vector<Account> accounts;
	selected_pl_accounts(accounts);
	if (accounts.empty())
	{
		// TODO HIGH PRIORITY Deal with this in a more user-friendly
		// way. For now, we just do this.
		wxMessageBox
		(	"Category to edit must first be selected in main window."
		);
		return;
	}
	assert (accounts.size() >= 1);
	Account account = accounts[0];
	assert
	(	super_type(account.account_type()) ==
		account_super_type::pl
	);
	AccountDialog account_dialog
	(	this,
		account,
		account_super_type::pl
	);
	if (account_dialog.ShowModal() == wxID_OK)
	{
		// TODO This will obliterate any contents of the TransactionCtrl.
		// Do we want this? We probably \e do want it to update the
		// AccountTypeCtrl and AccountCtrls in the TransactionCtrl; but
		// we don't really want it to obliterate everything else.
		m_top_panel->update_for(account);
	}
	return;
}

void
Frame::on_edit_ordinary_journal(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<OrdinaryJournal> journals;
	selected_ordinary_journals(journals);
	if (journals.empty())
	{
		// TODO It should be impossible for the user to even reach
		// here, as the menu item to edit a transaction should be
		// disabled unless a transaction is selected.
		wxMessageBox("No transaction is currently selected.");	
		return;	
	}
	else
	{
		assert (journals.size() >= 1);
		m_top_panel->configure_transaction_ctrl(journals[0]);
	}
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

void
Frame::selected_ordinary_journals(vector<OrdinaryJournal>& out) const
{
	m_top_panel->selected_ordinary_journals(out);
	return;
}

}  // namespace gui
}  // namespace phatbooks

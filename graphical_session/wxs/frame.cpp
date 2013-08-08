// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "frame.hpp"
#include "account.hpp"
#include "account_dialog.hpp"
#include "account_list_ctrl.hpp"
#include "application.hpp"
#include "app.hpp"
#include "b_string.hpp"
#include "budget_dialog.hpp"
#include "entry_list_ctrl.hpp"
#include "icon.xpm"
#include "draft_journal.hpp"
#include "ordinary_journal.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "top_panel.hpp"
#include <boost/optional.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/wx.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

#include <jewel/debug_log.hpp>  // for debugging / testing
#include <iostream>  // for debugging / testing
using std::cout;     // for debugging / testing
using std::endl;     // for debubbing / testing


namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(Frame, wxFrame)
	PHATBOOKS_EVT_ACCOUNT_EDITING
	(	wxID_ANY,
		Frame::on_account_editing_requested
	)
END_EVENT_TABLE()

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

	wxString const balance_sheet_account_concept_name = bstring_to_wx
	(	account_concept_name(account_super_type::balance_sheet)
	);
	wxString const pl_account_concept_name = bstring_to_wx
	(	account_concept_name(account_super_type::pl)
	);

	// Configure "new" menu
	m_new_menu->Append
	(	s_new_bs_account_id,
		wxString("New &") + balance_sheet_account_concept_name,
		wxString("Create a new asset or liability ") +
			balance_sheet_account_concept_name
	);
	m_new_menu->Append
	(	s_new_pl_account_id,
		wxString("New &") + pl_account_concept_name,
		wxString("Create a new revenue or expenditure ") +
			pl_account_concept_name
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
		wxString("Edit selected &") + balance_sheet_account_concept_name,
		wxString("Edit an exising asset or liability account")
	);
	m_edit_menu->Append
	(	s_edit_pl_account_id,
		wxString("Edit selected &") + pl_account_concept_name,
		wxString("Edit an existing revenue or expenditure ") +
			pl_account_concept_name
	);
	m_edit_menu->Append
	(	s_edit_ordinary_journal_id,
		wxString("Edit selected &ordinary transaction"),
		wxString("Edit an existing ordinary (non-recurring) transaction")
	);
	m_edit_menu->Append
	(	s_edit_draft_journal_id,
		wxString("Edit selected &recurring transaction"),
		wxString("Edit an exising recurring transaction")
	);
	m_edit_menu->Append
	(	s_edit_budget_id,
		wxString("Edit budget"),
		wxString("Edit budget for selected ") +
			pl_account_concept_name
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
		wxCommandEventHandler(Frame::on_menu_quit)
	);
	Connect
	(	s_new_bs_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_new_bs_account)
	);
	Connect
	(	s_new_pl_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_new_pl_account)
	);
	Connect
	(	s_new_transaction_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_new_transaction)
	);
	Connect
	(	s_edit_bs_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_edit_bs_account)
	);
	Connect
	(	s_edit_pl_account_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_edit_pl_account)
	);
	Connect
	(	s_edit_ordinary_journal_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_edit_ordinary_journal)
	);
	Connect
	(	s_edit_draft_journal_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_edit_draft_journal)
	);
	Connect
	(	s_edit_budget_id,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_edit_budget)
	);
	Connect
	(	wxID_ABOUT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::on_menu_about)
	);

#	if JEWEL_ON_WINDOWS
		Maximize();
#	endif

	m_top_panel = new TopPanel(this, m_database_connection);

}
	
void
Frame::on_menu_about(wxCommandEvent& event)
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
Frame::on_menu_quit(wxCommandEvent& event)
{
	// Destroy the frame
	Close();
	(void)event;  // Silence compiler warning re. unused parameter.
	return;
}

void
Frame::on_menu_new_bs_account(wxCommandEvent& event)
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
		m_top_panel->update_for_new(account);
	}
	return;
}

void
Frame::on_menu_new_pl_account(wxCommandEvent& event)
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
		m_top_panel->update_for_new(account);
	}
	return;
}

void
Frame::on_menu_new_transaction(wxCommandEvent& event)
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
Frame::on_menu_edit_bs_account(wxCommandEvent& event)
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
	edit_account(account);
	return;
}

void
Frame::on_menu_edit_pl_account(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	vector<Account> accounts;
	selected_pl_accounts(accounts);
	if (accounts.empty())
	{
		// TODO HIGH PRIORITY Deal with this in a more user-friendly
		// way. For now, we just do this.
		wxString const concept_name =
			bstring_to_wx(account_concept_name(account_super_type::pl, true));
		wxMessageBox
		(	concept_name +
			wxString(" to edit must first be selected in main window.")
		);
		return;
	}
	assert (accounts.size() >= 1);
	Account account = accounts[0];
	assert
	(	super_type(account.account_type()) ==
		account_super_type::pl
	);
	edit_account(account);
	return;
}

void
Frame::on_menu_edit_ordinary_journal(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<OrdinaryJournal> journals;
	selected_ordinary_journals(journals);
	if (journals.empty())
	{
		// TODO It should be impossible for the user to even reach
		// here, as the menu item to edit an ordinary transaction should
		// be disabled unless an ordinary transaction is selected.
		wxMessageBox("No transaction is currently selected.");	
		return;	
	}
	assert (journals.size() >= 1);
	m_top_panel->configure_transaction_ctrl(journals[0]);
	return;
}

void
Frame::on_menu_edit_draft_journal(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<DraftJournal> journals;
	selected_draft_journals(journals);
	if (journals.empty())
	{
		// TODO It should be impossible for the user to even reach
		// here, as the menu item to edit a recurring transaction
		// should be disabled unless a recurring transaction
		// is selected.
		wxMessageBox("No recurring transaction is currently selected.");
		return;
	}
	assert (journals.size() >= 1);
	m_top_panel->configure_transaction_ctrl(journals[0]);
	return;
}

void
Frame::on_menu_edit_budget(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<Account> accounts;
	selected_pl_accounts(accounts);
	if (accounts.empty())
	{
		// TODO HIGH PRIORITY Deal with this in a more user-friendly
		// way. For now, we just do this.
		wxString msg("To edit the budget for ");
		wxString const concept_name = bstring_to_wx
		(	account_concept_name
			(	account_super_type::pl,
				false,
				true
			)
		);
		msg += concept_name;
		msg += wxString(", ");
		msg += concept_name;
		msg += wxString(" must first be selected in the main window.");
		wxMessageBox(msg);
		return;
	}
	assert (accounts.size() >= 1);
	Account account = accounts[0];
	if (account == m_database_connection.balancing_account())
	{
		wxString msg("Budgets for the \"");
		msg += bstring_to_wx(account.name());
		msg += wxString("\" ");
		msg += bstring_to_wx(account_concept_name(account_super_type::pl));
		msg += wxString(" cannot be edited directly.");
		wxMessageBox(msg);
		return;
	}
	assert (super_type(account.account_type()) == account_super_type::pl);
	BudgetDialog budget_dialog(this, account);
	if (budget_dialog.ShowModal() == wxID_OK)
	{
		m_top_panel->update_for_amended_budget(account);
	}
	return;
}

void
Frame::on_account_editing_requested(PersistentObjectEvent& event)
{
	optional<PersistentObjectEvent::Id> const maybe_id = event.maybe_po_id();
	assert (maybe_id);
	Account account(m_database_connection, value(maybe_id));
	edit_account(account);
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

void
Frame::selected_draft_journals(vector<DraftJournal>& out) const
{
	m_top_panel->selected_draft_journals(out);
	return;
}

void
Frame::edit_account(Account& p_account)
{
	AccountDialog account_dialog
	(	this,
		p_account,
		super_type(p_account.account_type())
	);
	if (account_dialog.ShowModal() == wxID_OK)
	{
		// TODO This will obliterate any contents of the TransactionCtrl.
		// Do we want this? We probably \e do want it to update the
		// AccountTypeCtrl and AccountCtrls in the TransactionCtrl; but
		// we don't really want it to obliterate everything else.
		m_top_panel->update_for_amended(p_account);
	}
	return;
}


}  // namespace gui
}  // namespace phatbooks

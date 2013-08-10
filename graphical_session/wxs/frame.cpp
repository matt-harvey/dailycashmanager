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
#include "persistent_journal.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "top_panel.hpp"
#include <jewel/on_windows.hpp>
#include <wx/event.h>
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

BEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_MENU
	(	wxID_EXIT,
		Frame::on_menu_quit
	)
	EVT_MENU
	(	s_new_bs_account_id,
		Frame::on_menu_new_bs_account
	)
	EVT_MENU
	(	s_new_pl_account_id,
		Frame::on_menu_new_pl_account
	)
	EVT_MENU
	(	s_new_transaction_id,
		Frame::on_menu_new_transaction
	)
	EVT_MENU
	(	s_edit_bs_account_id,
		Frame::on_menu_edit_bs_account
	)
	EVT_MENU
	(	s_edit_pl_account_id,
		Frame::on_menu_edit_pl_account
	)
	EVT_MENU
	(	s_edit_ordinary_journal_id,
		Frame::on_menu_edit_ordinary_journal
	)
	EVT_MENU
	(	s_edit_draft_journal_id,
		Frame::on_menu_edit_draft_journal
	)
	EVT_MENU
	(	s_edit_budget_id,
		Frame::on_menu_edit_budget
	)
	EVT_MENU
	(	wxID_ABOUT,
		Frame::on_menu_about
	)
	PHATBOOKS_EVT_ACCOUNT_EDITING
	(	wxID_ANY,
		Frame::on_account_editing_requested
	)
	PHATBOOKS_EVT_JOURNAL_EDITING
	(	wxID_ANY,
		Frame::on_journal_editing_requested
	)
	PHATBOOKS_EVT_ACCOUNT_CREATED
	(	wxID_ANY,
		Frame::on_account_created_event
	)
	PHATBOOKS_EVT_ACCOUNT_EDITED
	(	wxID_ANY,
		Frame::on_account_edited_event
	)
	PHATBOOKS_EVT_JOURNAL_CREATED
	(	wxID_ANY,
		Frame::on_journal_created_event
	)
	PHATBOOKS_EVT_JOURNAL_EDITED
	(	wxID_ANY,
		Frame::on_journal_edited_event
	)
	PHATBOOKS_EVT_DRAFT_JOURNAL_DELETED
	(	wxID_ANY,
		Frame::on_draft_journal_deleted_event
	)
	PHATBOOKS_EVT_ORDINARY_JOURNAL_DELETED
	(	wxID_ANY,
		Frame::on_ordinary_journal_deleted_event
	)
	PHATBOOKS_EVT_DRAFT_ENTRY_DELETED
	(	wxID_ANY,
		Frame::on_draft_entry_deleted_event
	)
	PHATBOOKS_EVT_ORDINARY_ENTRY_DELETED
	(	wxID_ANY,
		Frame::on_ordinary_entry_deleted_event
	)
	PHATBOOKS_EVT_BUDGET_EDITED
	(	wxID_ANY,
		Frame::on_budget_edited_event
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
	// m_top_panel->SetFocus();  // WARNING This doesn't seem to have any effect
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
	edit_journal(journals[0]);
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
	edit_journal(journals[0]);
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
	budget_dialog.ShowModal();
	return;
}

void
Frame::on_account_editing_requested(PersistentObjectEvent& event)
{
	Account account(m_database_connection, event.po_id());
	edit_account(account);
	return;
}

void
Frame::on_journal_editing_requested(PersistentObjectEvent& event)
{
	PersistentJournal::Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		DraftJournal journal(m_database_connection, journal_id);
		edit_journal(journal);
	}
	else
	{
		OrdinaryJournal journal(m_database_connection, journal_id);
		edit_journal(journal);
	}
	return;
}

void
Frame::on_account_created_event(PersistentObjectEvent& event)
{
	Account const account(m_database_connection, event.po_id());
	m_top_panel->update_for_new(account);
	return;
}

void
Frame::on_account_edited_event(PersistentObjectEvent& event)
{
	// TODO This will obliterate any contents of the TransactionCtrl.
	// Do we want this? We probably \e do want it to update the
	// AccountTypeCtrl and AccountCtrls in the TransactionCtrl; but
	// we don't really want it to obliterate everything else.
	Account const account(m_database_connection, event.po_id());
	m_top_panel->update_for_amended(account);
	return;
}

void
Frame::on_journal_created_event(PersistentObjectEvent& event)
{
	assert (m_top_panel);
	PersistentJournal::Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		DraftJournal const journal(m_database_connection, journal_id);
		m_top_panel->update_for_new(journal);
	}
	else
	{
		OrdinaryJournal const journal(m_database_connection, journal_id);
		m_top_panel->update_for_new(journal);
	}
	return;
}

void
Frame::on_journal_edited_event(PersistentObjectEvent& event)
{
	// WARNING Repeats code from on_journal_created_event(...).
	assert (m_top_panel);
	PersistentJournal::Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		DraftJournal const journal(m_database_connection, journal_id);
		m_top_panel->update_for_amended(journal);
	}
	else
	{
		OrdinaryJournal const journal(m_database_connection, journal_id);
		m_top_panel->update_for_amended(journal);
	}
	return;
}

void
Frame::on_draft_journal_deleted_event(PersistentObjectEvent& event)
{
	assert (m_top_panel);
	m_top_panel->update_for_deleted_draft_journal(event.po_id());
	return;
}

void
Frame::on_ordinary_journal_deleted_event(PersistentObjectEvent& event)
{
	assert (m_top_panel);
	m_top_panel->update_for_deleted_ordinary_journal(event.po_id());
	return;
}

void
Frame::on_draft_entry_deleted_event(PersistentObjectEvent& event)
{
	// TODO The chain of functions that are now called (via m_top_panel)
	// expect a vector. This is now just pointlessly wasteful given
	// what we are now processing one event at a time!
	static vector<Entry::Id> doomed_ids(1, 0);
	assert (doomed_ids.size() == 1);
	doomed_ids[0] = event.po_id();
	assert (m_top_panel);
	m_top_panel->update_for_deleted_draft_entries(doomed_ids);
	return;
}

void
Frame::on_ordinary_entry_deleted_event(PersistentObjectEvent& event)
{
	// TODO The chain of functions that are now called (via m_top_panel)
	// a vector. This is now just pointlessly wasteful given
	// what we are now processing one event at a time!
	static vector<Entry::Id> doomed_ids(1, 0);
	assert (doomed_ids.size() == 1);
	doomed_ids[0] = event.po_id();
	assert (m_top_panel);
	m_top_panel->update_for_deleted_ordinary_entries(doomed_ids);
	return;
}

void
Frame::on_budget_edited_event(PersistentObjectEvent& event)
{
	Account account(m_database_connection, event.po_id());
	assert (m_top_panel);
	m_top_panel->update_for_amended_budget(account);
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
	account_dialog.ShowModal();
	return;
}

}  // namespace gui
}  // namespace phatbooks

/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "gui/frame.hpp"
#include "account.hpp"
#include "application.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_flags.hpp"
#include "gui/account_dialog.hpp"
#include "gui/account_list_ctrl.hpp"
#include "gui/app.hpp"
#include "gui/entry_list_ctrl.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/top_panel.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/wupdlock.h>
#include <wx/wx.h>
#include <vector>

#include "../images/icon.xpm"

using sqloxx::Handle;
using sqloxx::Id;
using std::vector;

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
	(	s_toggle_bs_account_show_hidden_id,
		Frame::on_menu_view_toggle_bs_account_show_hidden
	)
	EVT_MENU
	(	s_toggle_pl_account_show_hidden_id,
		Frame::on_menu_view_toggle_pl_account_show_hidden
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
	PHATBOOKS_EVT_RECONCILIATION_STATUS
	(	wxID_ANY,
		Frame::on_reconciliation_status_event
	)
END_EVENT_TABLE()


namespace
{
	wxString instruction_to_show_hidden
	(	AccountSuperType p_account_super_type,
		bool p_show_hidden
	)
	{
		wxString ret = (p_show_hidden? "Show": "Hide");
		ret += " hidden ";
		ret += account_concept_name
		(	p_account_super_type,
			AccountPhraseFlags().set(string_flags::pluralize)
		);
		return ret;
	}

}  // end anonymous namespace


Frame::Frame
(	wxString const& title,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxFrame
	(	0,
		wxID_ANY,
		title,
		wxDefaultPosition,
#		ifdef JEWEL_ON_WINDOWS
			wxDefaultSize
#		else
			wxSize
			(	wxSystemSettings::GetMetric(wxSYS_SCREEN_X),
				wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)
			)
#		endif
	),
	m_database_connection(p_database_connection),
	m_menu_bar(nullptr),
	m_file_menu(nullptr),
	m_new_menu(nullptr),
	m_edit_menu(nullptr),
	m_view_menu(nullptr),
	m_help_menu(nullptr),
	m_top_panel(nullptr)
{
	JEWEL_LOG_TRACE();

	// Set the frame icon
	// TODO MEDIUM PRIORITY I should use SetIcons to associate several icons of
	// different sizes with the Window. This avoids possible ugliness from
	// resizing of a single icon when different sizes are required in
	// different situations.
	SetIcon(wxIcon(icon_xpm));

	// Create menus
	JEWEL_LOG_TRACE();
	m_menu_bar = new wxMenuBar;
	m_file_menu = new wxMenu;
	m_new_menu = new wxMenu;
	m_edit_menu = new wxMenu;
	m_view_menu = new wxMenu;
	m_help_menu = new wxMenu;

	// Configure "file" menu
	JEWEL_LOG_TRACE();
	m_file_menu->Append
	(	wxID_EXIT,
		wxString("E&xit\tAlt-X"),
		wxString("Quit this program")
	);
	m_menu_bar->Append(m_file_menu, wxString("&File"));

	wxString const balance_sheet_account_concept_name =
		account_concept_name(AccountSuperType::balance_sheet);
	wxString const pl_account_concept_name =
		account_concept_name(AccountSuperType::pl);

	// Configure "new" menu
	JEWEL_LOG_TRACE();
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
	JEWEL_LOG_TRACE();
	m_edit_menu->Append
	(	s_edit_bs_account_id,
		wxString("Edit selected &") + balance_sheet_account_concept_name,
		wxString("Edit an existing asset or liability account")
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
	m_menu_bar->Append(m_edit_menu, wxString("&Edit"));
	
	// Configure "view" menu
	JEWEL_LOG_TRACE();
	m_view_menu->Append
	(	s_toggle_bs_account_show_hidden_id,
		instruction_to_show_hidden
		(	AccountSuperType::balance_sheet,
			true
		),
		wxEmptyString
	);
	m_view_menu->Append
	(	s_toggle_pl_account_show_hidden_id,
		instruction_to_show_hidden
		(	AccountSuperType::pl,
			true
		),
		wxEmptyString
	);
	m_menu_bar->Append(m_view_menu, wxString("&View"));

	// Configure "help" menu
	JEWEL_LOG_TRACE();
	m_help_menu->Append
	(	wxID_ABOUT,
		wxString("&About...\tF1"),
		wxString("Show about dialog")
	);
	m_menu_bar->Append(m_help_menu, wxString("&Help"));

	SetMenuBar(m_menu_bar);

#	ifdef JEWEL_ON_WINDOWS
		Maximize();
#	endif

	m_top_panel = new TopPanel(this, m_database_connection);
	JEWEL_LOG_TRACE();
}
	
void
Frame::on_menu_about(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler warning re. unused parameter.
	// TODO HIGH PRIORITY Put better message here
	wxString msg;
	msg.Printf
	(	"Hello and welcome to %s",
		Application::application_name()
	);
	wxMessageBox(msg);
	return;
}

void
Frame::on_menu_quit(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	// Destroy the frame
	Close();
	(void)event;  // Silence compiler warning re. unused parameter.
	return;
}

void
Frame::on_menu_new_bs_account(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler warning re. unused parameter.
	Handle<Account> const account(m_database_connection);
	AccountDialog account_dialog
	(	this,
		account,
		AccountSuperType::balance_sheet
	);
	account_dialog.ShowModal();
	return;
}

void
Frame::on_menu_new_pl_account(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler warning re. unused parameter.
	Handle<Account> const account(m_database_connection);
	AccountDialog account_dialog(this, account, AccountSuperType::pl);
	account_dialog.ShowModal();
	return;
}

void
Frame::on_menu_new_transaction(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<Handle<Account> > balance_sheet_accounts;
	selected_balance_sheet_accounts(balance_sheet_accounts);
	vector<Handle<Account> > pl_accounts;
	selected_pl_accounts(pl_accounts);
	m_top_panel->configure_transaction_ctrl();
	return;
}

void
Frame::on_menu_edit_bs_account(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();

	// TODO MEDIUM PRIORITY Factor out repeated code in the various member
	// functions that invoke an AccountDialog.

	(void)event;  // Silence compiler re. unused parameter.
	vector<Handle<Account> > accounts;
	selected_balance_sheet_accounts(accounts);
	if (accounts.empty())
	{
		wxMessageBox
		(	"Account to edit must first be selected in main window."
		);
		return;
	}
	JEWEL_ASSERT (accounts.size() >= 1);
	Handle<Account> account = accounts[0];
	JEWEL_ASSERT
	(	super_type(account->account_type()) ==
		AccountSuperType::balance_sheet
	);
	edit_account(account);
	return;
}

void
Frame::on_menu_edit_pl_account(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler re. unused parameter.
	vector<Handle<Account> > accounts;
	selected_pl_accounts(accounts);
	if (accounts.empty())
	{
		wxString const concept_name = account_concept_name
		(	AccountSuperType::pl,
			AccountPhraseFlags().set(string_flags::capitalize)
		);
		wxMessageBox
		(	concept_name +
			wxString(" to edit must first be selected in main window.")
		);
		return;
	}
	JEWEL_ASSERT (accounts.size() >= 1);
	Handle<Account> const account = accounts[0];
	JEWEL_ASSERT
	(	super_type(account->account_type()) ==
		AccountSuperType::pl
	);
	edit_account(account);
	return;
}

void
Frame::on_menu_edit_ordinary_journal(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<Handle<OrdinaryJournal> > journals;
	selected_ordinary_journals(journals);
	if (journals.empty())
	{
		// TODO MEDIUM PRIORITY It should be impossible for the user even to
		// reach here, as the menu item to edit an ordinary transaction should
		// be disabled (as in, "greyed out") unless an ordinary transaction is
		// selected.
		wxMessageBox("No transaction is currently selected.");	
		return;	
	}
	JEWEL_ASSERT (journals.size() >= 1);
	Handle<OrdinaryJournal> const journal = journals[0];
	edit_journal(journal);
	return;
}

void
Frame::on_menu_edit_draft_journal(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // Silence compiler warning re. unused parameter.
	vector<Handle<DraftJournal> > journals;
	selected_draft_journals(journals);
	if (journals.empty())
	{
		// TODO MEDIUM PRIORITY It should be impossible for the user to even
		// reach here, as the menu item to edit a recurring transaction
		// should be disabled (as in "greyed out") unless a recurring
		// transaction is selected.
		wxMessageBox("No recurring transaction is currently selected.");
		return;
	}
	JEWEL_ASSERT (journals.size() >= 1);
	edit_journal(journals[0]);
	return;
}

void
Frame::on_menu_view_toggle_bs_account_show_hidden(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();

	// TODO MEDIUM PRIORITY Factor out code duplicated here and in
	// on_menu_view_toggle_pl_account_show_hidden(...).
	AccountSuperType const stype =
		AccountSuperType::balance_sheet;
	
	bool const showing_hidden =
		m_top_panel->toggle_show_hidden_accounts(stype);
	bool const next_toggle_will_show_hidden = !showing_hidden;
	wxString const instruction_for_menu = instruction_to_show_hidden
	(	stype,
		next_toggle_will_show_hidden
	);
	m_view_menu->SetLabel(event.GetId(), instruction_for_menu);

	return;
}

void
Frame::on_menu_view_toggle_pl_account_show_hidden(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();

	AccountSuperType const stype =
		AccountSuperType::pl;

	bool const showing_hidden =
		m_top_panel->toggle_show_hidden_accounts(stype);
	bool const next_toggle_will_show_hidden = !showing_hidden;
	wxString const instruction_for_menu = instruction_to_show_hidden
	(	stype,
		next_toggle_will_show_hidden
	);
	m_view_menu->SetLabel(event.GetId(), instruction_for_menu);

	return;
}

void
Frame::on_account_editing_requested(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();

	Handle<Account> const account(m_database_connection, event.po_id());
	edit_account(account);
	return;
}

void
Frame::on_journal_editing_requested(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();

	Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		Handle<DraftJournal> const journal(m_database_connection, journal_id);
		edit_journal(journal);
	}
	else
	{
		Handle<OrdinaryJournal> const journal(m_database_connection, journal_id);
		edit_journal(journal);
	}
	return;
}

void
Frame::on_account_created_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	Handle<Account> const account(m_database_connection, event.po_id());
	m_top_panel->update_for_new(account);
	return;
}

void
Frame::on_account_edited_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	Handle<Account> const account(m_database_connection, event.po_id());
	m_top_panel->update_for_amended(account);
	return;
}

void
Frame::on_journal_created_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	JEWEL_ASSERT (m_top_panel);
	Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		Handle<DraftJournal> const journal(m_database_connection, journal_id);
		m_top_panel->update_for_new(journal);
	}
	else
	{
		Handle<OrdinaryJournal> const journal
		(	m_database_connection,
			journal_id
		);
		m_top_panel->update_for_new(journal);
	}
	return;
}

void
Frame::on_journal_edited_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);

	// TODO LOW PRIORITY Factor code duplicated from
	// on_journal_created_event(...).
	JEWEL_ASSERT (m_top_panel);
	Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		Handle<DraftJournal> const journal(m_database_connection, journal_id);
		m_top_panel->update_for_amended(journal);
	}
	else
	{
		Handle<OrdinaryJournal> const journal
		(	m_database_connection,
			journal_id
		);
		m_top_panel->update_for_amended(journal);
	}
	return;
}

void
Frame::on_draft_journal_deleted_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	JEWEL_ASSERT (m_top_panel);
	m_top_panel->update_for_deleted_draft_journal(event.po_id());
	return;
}

void
Frame::on_ordinary_journal_deleted_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	JEWEL_ASSERT (m_top_panel);
	m_top_panel->update_for_deleted_ordinary_journal(event.po_id());
	return;
}

void
Frame::on_draft_entry_deleted_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	// TODO LOW PRIORITY The chain of functions that are now called
	// (via m_top_panel) expect a vector. This is now just pointlessly wasteful
	// given what we are now processing one event at a time!
	wxWindowUpdateLocker const update_locker(this);
	static vector<Id> doomed_ids(1, 0);
	JEWEL_ASSERT (doomed_ids.size() == 1);
	doomed_ids[0] = event.po_id();
	JEWEL_ASSERT (m_top_panel);
	m_top_panel->update_for_deleted_draft_entries(doomed_ids);
	JEWEL_LOG_TRACE();
	return;
}

void
Frame::on_ordinary_entry_deleted_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	// TODO LOW PRIORITY The chain of functions that are now called
	// (via m_top_panel) a vector. This is now just pointlessly wasteful given
	// what we are now processing one event at a time!
	wxWindowUpdateLocker const update_locker(this);
	static vector<Id> doomed_ids(1, 0);
	JEWEL_ASSERT (doomed_ids.size() == 1);
	doomed_ids[0] = event.po_id();
	JEWEL_ASSERT (m_top_panel);
	m_top_panel->update_for_deleted_ordinary_entries(doomed_ids);
	return;
}

void
Frame::on_budget_edited_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	Handle<Account> account(m_database_connection, event.po_id());
	JEWEL_ASSERT (m_top_panel);
	m_top_panel->update_for_amended_budget(account);
	return;
}

void
Frame::on_reconciliation_status_event(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	Handle<Entry> const entry(m_database_connection, event.po_id());
	JEWEL_ASSERT (m_top_panel);
	m_top_panel->update_for_reconciliation_status(entry);
	return;
}

void
Frame::selected_balance_sheet_accounts(vector<Handle<Account> >& out) const
{
	JEWEL_LOG_TRACE();
	m_top_panel->selected_balance_sheet_accounts(out);
	return;
}

void
Frame::selected_pl_accounts(vector<Handle<Account> >& out) const
{
	JEWEL_LOG_TRACE();
	m_top_panel->selected_pl_accounts(out);
	return;
}

void
Frame::selected_ordinary_journals(vector<Handle<OrdinaryJournal> >& out) const
{
	JEWEL_LOG_TRACE();
	m_top_panel->selected_ordinary_journals(out);
	return;
}

void
Frame::selected_draft_journals(vector<Handle<DraftJournal> >& out) const
{
	JEWEL_LOG_TRACE();
	m_top_panel->selected_draft_journals(out);
	return;
}

void
Frame::edit_account(Handle<Account> const& p_account)
{
	JEWEL_LOG_TRACE();
	AccountDialog account_dialog
	(	this,
		p_account,
		super_type(p_account->account_type())
	);
	account_dialog.ShowModal();
	return;
}

}  // namespace gui
}  // namespace phatbooks

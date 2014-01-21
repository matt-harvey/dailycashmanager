/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gui/frame.hpp"
#include "account.hpp"
#include "app.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "persistent_journal.hpp"
#include "dcm_database_connection.hpp"
#include "repeater.hpp"
#include "string_conv.hpp"
#include "string_flags.hpp"
#include "gui/about_dialog.hpp"
#include "gui/account_dialog.hpp"
#include "gui/account_list_ctrl.hpp"
#include "gui/entry_list_ctrl.hpp"
#include "gui/envelope_transfer_dialog.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/top_panel.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/version.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/event.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/wupdlock.h>
#include <wx/wx.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../images/icon_48_48.xpm"

using sqloxx::Handle;
using sqloxx::Id;
using std::endl;
using std::ostringstream;
using std::stable_partition;
using std::string;
using std::vector;

namespace dcm
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
	(	s_new_envelope_transfer_id,
		Frame::on_menu_new_envelope_transfer
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
	(	wxID_HELP_CONTENTS,
		Frame::on_menu_help_contents
	)
	EVT_MENU
	(	wxID_ABOUT,
		Frame::on_menu_about
	)
	DCM_EVT_ACCOUNT_EDITING
	(	wxID_ANY,
		Frame::on_account_editing_requested
	)
	DCM_EVT_JOURNAL_EDITING
	(	wxID_ANY,
		Frame::on_journal_editing_requested
	)
	DCM_EVT_ACCOUNT_CREATED
	(	wxID_ANY,
		Frame::on_account_created_event
	)
	DCM_EVT_ACCOUNT_EDITED
	(	wxID_ANY,
		Frame::on_account_edited_event
	)
	DCM_EVT_JOURNAL_CREATED
	(	wxID_ANY,
		Frame::on_journal_created_event
	)
	DCM_EVT_JOURNAL_EDITED
	(	wxID_ANY,
		Frame::on_journal_edited_event
	)
	DCM_EVT_DRAFT_JOURNAL_DELETED
	(	wxID_ANY,
		Frame::on_draft_journal_deleted_event
	)
	DCM_EVT_ORDINARY_JOURNAL_DELETED
	(	wxID_ANY,
		Frame::on_ordinary_journal_deleted_event
	)
	DCM_EVT_DRAFT_ENTRY_DELETED
	(	wxID_ANY,
		Frame::on_draft_entry_deleted_event
	)
	DCM_EVT_ORDINARY_ENTRY_DELETED
	(	wxID_ANY,
		Frame::on_ordinary_entry_deleted_event
	)
	DCM_EVT_BUDGET_EDITED
	(	wxID_ANY,
		Frame::on_budget_edited_event
	)
	DCM_EVT_RECONCILIATION_STATUS
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
	DcmDatabaseConnection& p_database_connection
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
	SetIcon(wxIcon(icon_48_48_xpm));

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
	m_new_menu->Append
	(	s_new_envelope_transfer_id,
		wxString("New en&velope transfer \tAlt-V"),
		wxString("Transfer budget funds between envelopes")
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
	(	wxID_HELP_CONTENTS,
		wxString("&Help...\tF1"),
		wxString("Show help")
	);
	m_help_menu->Append
	(	wxID_ABOUT,
		wxString("About"),
		wxString("Show information about this application")
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
	m_top_panel->configure_transaction_ctrl();
	return;
}

void
Frame::on_menu_new_envelope_transfer(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // silence compiler re. unused parameter
	JEWEL_ASSERT (m_top_panel);
	ProtoJournal proto_journal = m_top_panel->make_proto_envelope_transfer();
	JEWEL_ASSERT (proto_journal.entries().size() == 2);
	edit_envelope_transfer(proto_journal);
	JEWEL_LOG_TRACE();
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
		wxMessageBox("Account to edit must first be selected in main window.");
		return;
	}
	JEWEL_ASSERT (accounts.size() >= 1);
	Handle<Account> account = accounts[0];
	JEWEL_ASSERT
	(	account->account_super_type() ==
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
	JEWEL_ASSERT (account->account_super_type() == AccountSuperType::pl);
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
	auto const stype = AccountSuperType::balance_sheet;
	bool const showing_hidden = m_top_panel->toggle_show_hidden_accounts(stype);
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
	auto const stype = AccountSuperType::pl;
	bool const showing_hidden = m_top_panel->toggle_show_hidden_accounts(stype);
	bool const next_toggle_will_show_hidden = !showing_hidden;
	wxString const instruction_for_menu = instruction_to_show_hidden
	(	stype,
		next_toggle_will_show_hidden
	);
	m_view_menu->SetLabel(event.GetId(), instruction_for_menu);
	return;
}

void
Frame::on_menu_help_contents(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // silence compiler re. unused parameter.
	auto app = dynamic_cast<App*>(wxTheApp);
	JEWEL_ASSERT (app);
	app->display_help_contents();
	return;
}

void
Frame::on_menu_about(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // silence compiler re. unused parameter
	ostringstream version_stream;
	version_stream << App::version();
	wxAboutDialogInfo info;
	info.SetName(App::application_name());
	info.SetVersion(std8_to_wx(version_stream.str()));
	SetIcon(wxIcon(icon_48_48_xpm));
	info.SetWebSite("http://dailycashmanager.sourceforge.net");
	AboutDialog dialog(info, this, App::copyright());
	dialog.ShowModal();
	return;
}

void
Frame::on_account_editing_requested(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	Handle<Account> const account(m_database_connection, event.po_id());
	edit_account(account);
	JEWEL_LOG_TRACE();
	return;
}

void
Frame::on_journal_editing_requested(PersistentObjectEvent& event)
{
	JEWEL_LOG_TRACE();
	wxWindowUpdateLocker const update_locker(this);
	Id const journal_id = event.po_id();
	if (journal_id_is_draft(m_database_connection, journal_id))
	{
		Handle<DraftJournal> const journal
		(	m_database_connection,
			journal_id
		);
		edit_journal(journal);
	}
	else
	{
		Handle<OrdinaryJournal> const journal
		(	m_database_connection,
			journal_id
		);
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
Frame::report_repeater_firing_results
(	vector<RepeaterFiringResult> p_results
)
{
	JEWEL_LOG_TRACE();
	if (p_results.empty())
	{	
		JEWEL_LOG_TRACE();
		return;
	}
	JEWEL_LOG_TRACE();
	auto const end_all = p_results.end();
	auto const end_normal = stable_partition
	(	p_results.begin(),
		end_all,
		[this](RepeaterFiringResult const& r)
		{
			Handle<DraftJournal> const dj
			(	this->m_database_connection,
				r.draft_journal_id
			);
			return dj != m_database_connection.budget_instrument();
		}
	);
	auto const end_successful = stable_partition
	(	p_results.begin(),
		end_normal,
		[](RepeaterFiringResult const& r){ return r.successful; }
	);

	// p_results is now ordered in three bands, as follows:
	//  - firing attempts of non-budget-instrument DraftJournals that
	//      were successful; then
	//  - firing attemptes of non-budget-instrument DraftJournals that
	//      were unsuccessful; then
	//  - firing attempts of the budget instrument DraftJournal.

	auto it = p_results.begin();
	if (it != end_successful)
	{
		JEWEL_LOG_TRACE();
		ostringstream oss;
		oss << "The following transaction"
		    << ((end_successful - it == 1)? " has": "s have")
			<< " been automatically "
		    << "recorded since the last session:\n\n";
		for ( ; it != end_successful; ++it)
		{
			Handle<DraftJournal> const dj
			(	m_database_connection,
				it->draft_journal_id
			);
			JEWEL_ASSERT (it->successful);
			oss << "\"" << dj->name() << "\""
				<< " was recorded on "
				<< date_format_wx(it->firing_date);
			if (end_successful - it == 1) oss << ".\n";
			else oss << ";\n";
		}
		// TODO MEDIUM PRIORITY Make this message a bit nicer looking.
		JEWEL_LOG_TRACE();
		wxMessageBox(std8_to_wx(oss.str()));
	}
	if (it != end_normal)
	{
		JEWEL_LOG_TRACE();
		// TODO LOW PRIORITY Provide some more helpful information to
		// the user on what to do about the transactions not being
		// posted (though it's very unlikely this will ever occur for
		// any given user).
		ostringstream oss2;
		if (end_normal - it == 1)
		{
			oss2 << "The following transaction was scheduled to be "
			     << "automatically recorded, but wasn't. This is likely to be "
				 << "due to the amount involved being too large to be safely "
				 << "processed by the application:\n\n";
		}
		else
		{
			oss2 << "The following transactions were scheduled to be "
			     << "automatically recorded, but weren't. This is likely to be "
				 << "due to the amounts involved being too large to be safely "
				 << "processed by the application:\n\n";
		}
		for ( ; it != end_normal ; ++it)
		{
			Handle<DraftJournal> const dj
			(	m_database_connection,
				it->draft_journal_id
			);
			JEWEL_ASSERT (!it->successful);
			oss2 << "\"" << dj->name() << "\""
			     << " was not recorded, though it was next scheduled for "
				 << date_format_wx(it->firing_date);
			if (end_normal - it == 1) oss2 << ".\n";
			else oss2 << ";\n";
		}
		wxMessageBox
		(	std8_to_wx(oss2.str()),
			"WARNING",
			wxOK | wxICON_EXCLAMATION
		);
	}
	JEWEL_ASSERT (end_all >= end_normal);
	JEWEL_ASSERT (it == end_normal);
	if (it != end_all)
	{
		// Then we have budget instrument firing attempts to deal
		// with.
		for ( ; it != end_all; ++it)
		{
			Handle<DraftJournal> const dj
			(	m_database_connection,
				it->draft_journal_id
			);
			JEWEL_ASSERT (dj == m_database_connection.budget_instrument());

			// for budget instrument, we only report unsuccessful firings
			if (!it->successful)
			{
				wxMessageBox
				(	"The regular top-up of envelopes with budget amounts "
						"has not occurred.\nThis is likely due to some budget "
						"amounts being too large for the application to "
						"process safely.\n",
					"WARNING",
					wxOK | wxICON_EXCLAMATION
				);
			}
		}
	}
	JEWEL_LOG_TRACE();
	return;
}

void
Frame::edit_account(Handle<Account> const& p_account)
{
	JEWEL_LOG_TRACE();
	AccountDialog account_dialog
	(	this,
		p_account,
		p_account->account_super_type()
	);
	account_dialog.ShowModal();
	JEWEL_LOG_TRACE();
	return;
}

void
Frame::edit_envelope_transfer(ProtoJournal& p_journal)
{
	JEWEL_LOG_TRACE();
	EnvelopeTransferDialog envelope_transfer_dialog
	(	this,
		p_journal,
		m_database_connection
	);
	envelope_transfer_dialog.ShowModal();
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace dcm

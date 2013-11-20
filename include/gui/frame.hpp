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

#ifndef GUARD_frame_hpp_873675392881816
#define GUARD_frame_hpp_873675392881816

#include "repeater.hpp"
#include "top_panel.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <wx/menu.h>
#include <wx/wx.h>
#include <wx/string.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class DraftJournal;
class PhatbooksDatabaseConnection;
class ProtoJournal;

namespace gui
{

class PersistentObjectEvent;

// End forward declarations

/**
 * Main top-level widget for application. This in turn houses the TopPanel,
 * which in turn houses almost everything else. This is a fairly conventional
 * arrangement for wxWidgets applications.
 */
class Frame: public wxFrame
{
public:

	Frame
	(	wxString const& title,
		PhatbooksDatabaseConnection& p_database_connection
	);

	Frame(Frame const&) = delete;
	Frame(Frame&&) = delete;
	Frame& operator=(Frame const&) = delete;
	Frame& operator=(Frame&&) = delete;
	~Frame() = default;

	/**
	 * Populates \e out with all the balance sheet Accounts currently
	 * selected by the user in the main window.
	 */
	void selected_balance_sheet_accounts
	(	std::vector<sqloxx::Handle<Account> >& out
	) const;

	/**
	 * Populates \e out with all the P&L Accounts currently selected
	 * by the user in the main window.
	 */
	void selected_pl_accounts
	(	std::vector<sqloxx::Handle<Account> >& out
	) const;

	/**
	 * Populates \e out with all the OrdinaryJournals currently
	 * selected by the user in the main window.
	 */
	void selected_ordinary_journals
	(	std::vector<sqloxx::Handle<OrdinaryJournal> >& out
	) const;

	/**
	 * Populates \e out with all the DraftJournals currently
	 * selected by the user in the main window.
	 */
	void selected_draft_journals
	(	std::vector<sqloxx::Handle<DraftJournal> >& out
	) const;

	/**
	 * Inform the Frame regarding the results of attempting
	 * to fire Repeaters. The Frame then takes responsibility
	 * for reporting these results to the user (or not) at a
	 * suitable juncture (which may or may not be immediately).
	 *
	 * NOTE Passing by value is deliberate.
	 */
	void report_repeater_firing_results
	(	std::vector<RepeaterFiringResult> p_results
	);

private:

	// Event handlers - menu selections
	void on_menu_quit(wxCommandEvent& event);
	void on_menu_new_bs_account(wxCommandEvent& event); 
	void on_menu_new_pl_account(wxCommandEvent& event);
	void on_menu_new_transaction(wxCommandEvent& event);
	void on_menu_new_envelope_transfer(wxCommandEvent& event);
	void on_menu_edit_bs_account(wxCommandEvent& event); 
	void on_menu_edit_pl_account(wxCommandEvent& event);
	void on_menu_edit_ordinary_journal(wxCommandEvent& event);
	void on_menu_edit_draft_journal(wxCommandEvent& event);
	void on_menu_view_toggle_bs_account_show_hidden(wxCommandEvent& event);
	void on_menu_view_toggle_pl_account_show_hidden(wxCommandEvent& event);

	// Event handlers - other - handle PersistentObject editing requests
	// fired.
	void on_account_editing_requested(PersistentObjectEvent& event);	
	void on_journal_editing_requested(PersistentObjectEvent& event);

	// Event handlers - other - handle notifications re. edited,
	// created or deleted PersistentObjects.
	void on_account_created_event(PersistentObjectEvent& event);
	void on_account_edited_event(PersistentObjectEvent& event);
	void on_journal_created_event(PersistentObjectEvent& event);
	void on_journal_edited_event(PersistentObjectEvent& event);
	void on_draft_journal_deleted_event(PersistentObjectEvent& event);
	void on_ordinary_journal_deleted_event(PersistentObjectEvent& event);
	void on_draft_entry_deleted_event(PersistentObjectEvent& event);
	void on_ordinary_entry_deleted_event(PersistentObjectEvent& event);
	void on_budget_edited_event(PersistentObjectEvent& event);
	void on_reconciliation_status_event(PersistentObjectEvent& event);

	// The actual function which conducts Account editing.
	void edit_account(sqloxx::Handle<Account> const& p_account);

	// The actual function which conducts Journal editing. JournalType
	// must be either sqloxx::Handle<OrdinaryJournal> or
	// sqloxx::Handle<DraftJournal>.
	template <typename JournalType>
	void edit_journal(JournalType const& p_journal);

	// The actual function which conducts Journal editing, where the
	// TransactionType is TransactionType::envelope, and the Journal
	// is a ProtoJournal
	void edit_envelope_transfer(ProtoJournal& p_journal);

	static int const s_new_bs_account_id = wxID_HIGHEST + 1;
	static int const s_new_pl_account_id = s_new_bs_account_id + 1;
	static int const s_new_transaction_id = s_new_pl_account_id + 1;
	static int const s_new_envelope_transfer_id = s_new_transaction_id + 1;
	static int const s_edit_bs_account_id = s_new_envelope_transfer_id + 1;
	static int const s_edit_pl_account_id = s_edit_bs_account_id + 1;
	static int const s_edit_ordinary_journal_id = s_edit_pl_account_id + 1;
	static int const s_edit_draft_journal_id = s_edit_ordinary_journal_id + 1;
	static int const s_toggle_bs_account_show_hidden_id =
		s_edit_draft_journal_id + 1;
	static int const s_toggle_pl_account_show_hidden_id =
		s_toggle_bs_account_show_hidden_id + 1;

	PhatbooksDatabaseConnection& m_database_connection;

	wxMenuBar* m_menu_bar;
	wxMenu* m_file_menu;
	wxMenu* m_new_menu;
	wxMenu* m_edit_menu;
	wxMenu* m_view_menu;
	TopPanel* m_top_panel;

	DECLARE_EVENT_TABLE()
};


// IMPLEMENT MEMBER FUNCTION TEMPLATES

template <typename JournalHandleType>
void
Frame::edit_journal(JournalHandleType const& p_journal)
{
	using std::is_same;
	using sqloxx::Handle;
	static_assert
	(	is_same<JournalHandleType, Handle<DraftJournal> >::value ||
		is_same<JournalHandleType, Handle<OrdinaryJournal> >::value,
		"Type passed to Frame::edit_journal other than " 
		"sqloxx::Handle<DraftJournal> or sqloxx::Handle<OrdinaryJournal>."
	);
	JEWEL_ASSERT (m_top_panel);
	// m_top_panel->SetFocus();  // This doesn't seem to have any effect...
	m_top_panel->configure_transaction_ctrl(p_journal);
	// TODO MEDIUM PRIORITY Bring the focus into the TransactionCtrl so the user
	// can start populating the TransactionCtrl immediately without having to
	// click into it.
	return;
}

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp_873675392881816

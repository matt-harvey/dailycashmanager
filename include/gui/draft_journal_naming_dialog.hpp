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

#ifndef GUARD_draft_journal_naming_dialog_hpp_17749934977735363
#define GUARD_draft_journal_naming_dialog_hpp_17749934977735363

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class TransactionCtrl;

// End forward declarations


/**
 * Widget for extracting a name from the user, for a DraftJournal created
 * via the GUI.
 */
class DraftJournalNamingDialog: public wxDialog
{
public:
	
	DraftJournalNamingDialog
	(	TransactionCtrl* p_parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

	DraftJournalNamingDialog(DraftJournalNamingDialog const&) = delete;
	DraftJournalNamingDialog(DraftJournalNamingDialog&&) = delete;
	DraftJournalNamingDialog& operator=(DraftJournalNamingDialog const&)
		= delete;
	DraftJournalNamingDialog& operator=(DraftJournalNamingDialog&&) = delete;
	virtual ~DraftJournalNamingDialog();

	wxString draft_journal_name() const;

private:

	void set_draft_journal_name(wxString const& p_name);

	void on_ok_button_click(wxCommandEvent& event);

	wxFlexGridSizer* m_top_sizer;
	wxTextCtrl* m_name_ctrl;
	wxButton* m_ok_button;
	PhatbooksDatabaseConnection& m_database_connection;
	wxString m_draft_journal_name;

	DECLARE_EVENT_TABLE()

};  // class DraftJournalNamingDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_draft_journal_naming_dialog_hpp_17749934977735363

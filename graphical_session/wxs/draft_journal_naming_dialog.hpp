// Copyright (c) 2013, Matthew Harvey. All rights reserved.

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

	virtual ~DraftJournalNamingDialog() = default;

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

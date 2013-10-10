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

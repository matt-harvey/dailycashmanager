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


#include "gui/draft_journal_naming_dialog.hpp"
#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "gui/sizing.hpp"
#include "gui/transaction_ctrl.hpp"
#include <jewel/assert.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(DraftJournalNamingDialog, wxDialog)
	EVT_BUTTON
	(	wxID_OK,
		DraftJournalNamingDialog::on_ok_button_click
	)
END_EVENT_TABLE()

DraftJournalNamingDialog::DraftJournalNamingDialog
(	TransactionCtrl* p_parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxDialog
	(	p_parent,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize
	),
	m_top_sizer(nullptr),
	m_name_ctrl(nullptr),
	m_ok_button(nullptr),
	m_database_connection(p_database_connection),
	m_draft_journal_name("")
{
	m_top_sizer =
		new wxFlexGridSizer(2, 4, wxSize(standard_gap(), standard_gap()));
	SetSizer(m_top_sizer);

	// Row 0

	m_top_sizer->AddSpacer(0);

	wxStaticText* label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Enter a unique name for this recurring transaction:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(label);

	m_top_sizer->AddSpacer(0);
	m_top_sizer->AddSpacer(standard_border());
	
	// Row 1

	m_top_sizer->AddSpacer(0);

	m_name_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(450, wxDefaultSize.y)
	);
	m_top_sizer->Add(m_name_ctrl);
	
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&OK"),
		wxDefaultPosition,
		wxSize(wxDefaultSize.x, m_name_ctrl->GetSize().GetHeight())
	);
	m_top_sizer->Add(m_ok_button);

	m_top_sizer->AddSpacer(0);

	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	CentreOnScreen();
}

DraftJournalNamingDialog::~DraftJournalNamingDialog()
{
}

wxString
DraftJournalNamingDialog::draft_journal_name() const
{
	return m_draft_journal_name;
}

void
DraftJournalNamingDialog::set_draft_journal_name(wxString const& p_name)
{
	m_draft_journal_name = p_name;
	return;
}


void
DraftJournalNamingDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	wxString const trimmed_name = m_name_ctrl->GetValue().Trim();
	if (trimmed_name.IsEmpty())
	{
		wxMessageBox("Transaction name cannot be blank.");
		return;
	}
	JEWEL_ASSERT (!trimmed_name.IsEmpty());
	wxString const name = trimmed_name;
	if (DraftJournal::exists(m_database_connection, name))
	{
		wxString msg("A transaction named \"");
		msg += name;
		msg += wxString("\" already exists.");
		wxMessageBox(msg);
		return;
	}
	JEWEL_ASSERT (!DraftJournal::exists(m_database_connection, name));
	set_draft_journal_name(name);
	EndModal(wxID_OK);
	return;
}

}  // namespace gui
}  // namespace phatbooks

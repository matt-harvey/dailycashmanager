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

#ifndef GUARD_envelope_transfer_dialog_hpp_5707053959233246
#define GUARD_envelope_transfer_dialog_hpp_5707053959233246

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/window.h>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;
class ProtoJournal;

namespace gui
{

class AccountCtrl;
class DecimalTextCtrl;

// end forward declarations

/**
 * Dialog in which user creates a new PersistentJournal of
 * TransactionType::envelope.
 */
class EnvelopeTransferDialog: public wxDialog
{
public:

	/**
	 * p_journal must have all its attributes already initialized and must have
	 * exactly two Entries, and all the Entries' attributes should have
	 * been initialized already. All the Entries' Accounts should be of
	 * AccountSuperType::pl.
	 */
	EnvelopeTransferDialog
	(	wxWindow* p_parent,
		ProtoJournal& p_journal,
		PhatbooksDatabaseConnection& p_database_connection
	);
	EnvelopeTransferDialog() = delete;
	EnvelopeTransferDialog(EnvelopeTransferDialog const& rhs) = delete;
	EnvelopeTransferDialog(EnvelopeTransferDialog&& rhs) = delete;
	EnvelopeTransferDialog& operator=(EnvelopeTransferDialog const& rhs) =
		delete;
	EnvelopeTransferDialog& operator=(EnvelopeTransferDialog&& rhs) = delete;
	virtual ~EnvelopeTransferDialog();

private:

	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	void update_proto_journal_from_dialog() const;

	wxGridBagSizer* m_top_sizer;
	AccountCtrl* m_source_account_ctrl;
	AccountCtrl* m_destination_account_ctrl;
	DecimalTextCtrl* m_amount_ctrl;
	wxButton* m_cancel_button;
	wxButton* m_ok_button;
	ProtoJournal& m_journal;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE();

};  // class EnvelopeTransferDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_envelope_transfer_dialog_hpp_5707053959233246

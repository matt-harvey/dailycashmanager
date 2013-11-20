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

#include "gui/envelope_transfer_dialog.hpp"
#include "account_type.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "ordinary_journal.hpp"
#include "transaction_side.hpp"
#include "gui/account_ctrl.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/sizing.hpp"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>

using std::vector;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(EnvelopeTransferDialog, wxDialog)
	EVT_BUTTON
	(	wxID_OK,
		EnvelopeTransferDialog::on_ok_button_click
	)
	EVT_BUTTON
	(	wxID_CANCEL,
		EnvelopeTransferDialog::on_cancel_button_click
	)
END_EVENT_TABLE()

EnvelopeTransferDialog::EnvelopeTransferDialog
(	wxWindow* p_parent,
	ProtoJournal& p_journal,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_top_sizer(nullptr),
	m_source_account_ctrl(nullptr),
	m_destination_account_ctrl(nullptr),
	m_amount_ctrl(nullptr),
	m_cancel_button(nullptr),
	m_ok_button(nullptr),
	m_journal(p_journal),
	m_database_connection(p_database_connection)
{
	JEWEL_ASSERT (m_journal.entries().size() == 2);

	int current_row = 0;

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());	
	SetSizer(m_top_sizer);

	vector<AccountType> const available_account_types =
		account_types(AccountSuperType::pl);

	for (auto const& entry: m_journal.entries())
	{
		wxString account_label_string;
		AccountCtrl** account_ctrl_ptr_ptr = nullptr;
		switch (entry->transaction_side())
		{
		case TransactionSide::source:
			account_label_string = "Source";
			account_ctrl_ptr_ptr = &m_source_account_ctrl;
			break;
		case TransactionSide::destination:
			account_label_string = "Destination";
			account_ctrl_ptr_ptr = &m_destination_account_ctrl;
			break;
		default:
			JEWEL_HARD_ASSERT (false);
		}
		JEWEL_ASSERT (account_ctrl_ptr_ptr != nullptr);
		wxStaticText* account_label = new wxStaticText
		(	this,
			wxID_ANY,
			account_label_string,
			wxDefaultPosition,
			wxSize(medium_width(), wxDefaultSize.GetY()),
			wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
		);
		m_top_sizer->Add
		(	account_label,
			wxGBPosition(current_row, 1),
			wxDefaultSpan,
			wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
		);
		auto const account = entry->account();
		JEWEL_ASSERT (account->account_super_type() == AccountSuperType::pl);
		*account_ctrl_ptr_ptr = new AccountCtrl
		(	this,
			wxID_ANY,
			wxSize(medium_width(), wxDefaultSize.GetY()),
			available_account_types,
			m_database_connection
		);
		(*account_ctrl_ptr_ptr)->set_account(entry->account());
		m_top_sizer->Add
		(	*account_ctrl_ptr_ptr,
			wxGBPosition(current_row, 2),
			wxGBSpan(1, 2),
			wxALIGN_LEFT
		);
		++current_row;
	}
	
	// TODO
		
}

EnvelopeTransferDialog::~EnvelopeTransferDialog() = default;

void
EnvelopeTransferDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	// TODO
}

void
EnvelopeTransferDialog::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	// TODO
}


}  // namespace gui
}  // namespace phatbooks

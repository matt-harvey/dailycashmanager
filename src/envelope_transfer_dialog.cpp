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
#include "gui/decimal_text_ctrl.hpp"
#include "gui/frame.hpp"
#include "gui/persistent_object_event.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "dcm_database_connection.hpp"
#include "proto_journal.hpp"
#include "transaction_side.hpp"
#include "gui/account_ctrl.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/sizing.hpp"
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>

using sqloxx::Handle;
using std::vector;

namespace dcm
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
	DcmDatabaseConnection& p_database_connection
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
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_journal.entries().size() == 2);

	int current_row = 0;

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());	
	SetSizer(m_top_sizer);

	JEWEL_LOG_TRACE();
	wxStaticText* amount_label = new wxStaticText
	(	this,
		wxID_ANY,
		"Amount",
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	amount_label,
		wxGBPosition(current_row, 1),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	JEWEL_LOG_TRACE();
	m_amount_ctrl = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), wxDefaultSize.y),
		m_database_connection.default_commodity()->precision(),
		false
	);
	m_top_sizer->Add
	(	m_amount_ctrl,
		wxGBPosition(current_row, 2),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	++current_row;
	JEWEL_LOG_TRACE();

	vector<AccountType> const available_account_types =
		account_types(AccountSuperType::pl);

	JEWEL_LOG_TRACE();
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
			wxDefaultSize,
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
			wxSize(large_width(), m_amount_ctrl->GetSize().y),
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
	JEWEL_LOG_TRACE();
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(medium_width(), m_amount_ctrl->GetSize().y),
		wxALIGN_RIGHT
	);
	m_top_sizer->Add
	(	m_cancel_button,
		wxGBPosition(current_row, 2),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	JEWEL_LOG_TRACE();
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&OK"),
		wxDefaultPosition,
		wxSize(medium_width(), m_amount_ctrl->GetSize().y)
	);
	m_top_sizer->Add
	(	m_ok_button,
		wxGBPosition(current_row, 3),
		wxDefaultSpan,
		wxALIGN_LEFT
	);
	JEWEL_LOG_TRACE();
	wxStaticText* dummy = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_top_sizer->Add(dummy, wxGBPosition(current_row, 4));
	m_top_sizer->Fit(this);
	JEWEL_LOG_TRACE();
	m_top_sizer->SetSizeHints(this);
	CentreOnScreen();
	Layout();
	JEWEL_LOG_TRACE();
}

EnvelopeTransferDialog::~EnvelopeTransferDialog() = default;

void
EnvelopeTransferDialog::on_ok_button_click(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // silence compiler re. unused parameter

	// TODO MEDIUM PRIORITY Address tiny change of
	// DecimalUnaryMinusException being thrown here.
	update_proto_journal_from_dialog();

	Handle<OrdinaryJournal> const oj(m_database_connection);
	oj->mimic(m_journal);
	oj->set_date(today());
	try
	{
		oj->save();
	}
	catch (JournalOverflowException&)
	{
		// TODO MEDIUM PRIORITY Make this message more helpful.
		// It might be that the transaction could be posted if
		// the user rolled over to a new
		// database. (And it would be good if there were an automatic
		// way of rolling over.) But it may be that there is no way
		// to accommodate the amount they want to post - even in a new
		// database. If that's the case, this needs to be communicated
		// honestly to the user.
		wxMessageBox
		(	"Transfer could not be saved. It may be that the amount "
			"is too large for it to be processed by the application."
		);
		return;
	}
	auto const frame = dynamic_cast<Frame*>(wxTheApp->GetTopWindow());
	JEWEL_ASSERT (frame);
	JEWEL_ASSERT (oj->has_id());
	PersistentObjectEvent::fire
	(	frame,
		DCM_JOURNAL_CREATED_EVENT,
		oj->id()
	);
	EndModal(wxID_OK);
	JEWEL_LOG_TRACE();
	return;
}

void
EnvelopeTransferDialog::on_cancel_button_click(wxCommandEvent& event)
{
	JEWEL_LOG_TRACE();
	(void)event;  // silence compiler re. unused parameter
	EndModal(wxID_CANCEL);
	JEWEL_LOG_TRACE();
	return;
}

void
EnvelopeTransferDialog::update_proto_journal_from_dialog() const
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_journal.entries().size() == 2);
	auto const se = m_journal.entries()[0];
	auto const de = m_journal.entries()[1];
	JEWEL_ASSERT (se->transaction_side() == TransactionSide::source);
	JEWEL_ASSERT (de->transaction_side() == TransactionSide::destination);
	se->set_account(m_source_account_ctrl->account());
	de->set_account(m_destination_account_ctrl->account());
	auto const amount = m_amount_ctrl->amount();
	se->set_amount(amount);
	de->set_amount(-amount);
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace dcm

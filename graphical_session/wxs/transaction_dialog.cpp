// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_dialog.hpp"
#include "account.hpp"
#include "date.hpp"
#include "date_validator.hpp"
#include "decimal_text_ctrl.hpp"
#include "decimal_validator.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/calctrl.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <cassert>
#include <iostream>
#include <vector>

using jewel::Decimal;
using std::endl;
using std::vector;

namespace phatbooks
{
namespace gui
{



BEGIN_EVENT_TABLE(TransactionDialog, wxDialog)
	EVT_BUTTON
	(	wxID_OK,
		TransactionDialog::on_ok_button_click
	)
END_EVENT_TABLE()


TransactionDialog::TransactionDialog(vector<Account> const& p_accounts):
	wxDialog
	(	0,  // Null parent implies top-level window is parent
		wxID_ANY,
		"New transaction",
		wxDefaultPosition,
		wxDefaultSize
		// , wxRESIZE_BORDER
	),
	m_top_sizer(0),
	m_date_ctrl(0),
	m_ok_button(0),
	m_cancel_button(0)
{
	assert (m_amount_boxes.empty());

	// We construct m_ok_button first as we want to be able to refer to its
	// size when sizing certain other controls below. But we will not add
	// the OK button to m_top_sizer till later.
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&OK"),
		wxDefaultPosition,
		wxDefaultSize
	);
	wxSize const ok_button_size = m_ok_button->GetSize();

	// Top sizer
	m_top_sizer = new wxFlexGridSizer(p_accounts.size() + 3, 3, 0, 0);

	// Column titles
	wxStaticText* header0 = new wxStaticText(this, wxID_ANY, "Account");
	wxStaticText* header1 = new wxStaticText(this, wxID_ANY, "Comment");
	wxStaticText* header2 = new wxStaticText(this, wxID_ANY, "Amount");
	m_top_sizer->Add(header0, 2, wxLEFT | wxBOTTOM, 10);
	m_top_sizer->Add(header1, 3, wxLEFT | wxBOTTOM, 10);
	m_top_sizer->Add(header2, 2, wxLEFT | wxBOTTOM, 10);

	// Rows for entering Entry details
	typedef vector<Account>::size_type Size;
	Size const sz = p_accounts.size();
	for (Size id = s_min_entry_row_id, i = 0 ; i != sz; ++i, ++id)
	{
		Account const account = p_accounts[i];
		wxStaticText* account_name_text = new wxStaticText
		(	this,
			id,
			bstring_to_wx(account.name()),
			wxDefaultPosition,
			wxDefaultSize
		);
		wxSize const account_name_text_size = account_name_text->GetSize();
		wxTextCtrl* comment_ctrl = new wxTextCtrl
		(	this,
			id,
			wxEmptyString,
			wxDefaultPosition,
			wxSize(ok_button_size.x * 4.5, account_name_text_size.y * 1.2),
			wxALIGN_LEFT
		);
		Decimal::places_type const precision =
			account.commodity().precision();
		DecimalTextCtrl* entry_ctrl = new DecimalTextCtrl
		(	this,
			id,
			wxSize(ok_button_size.x * 1.5, account_name_text_size.y * 1.2),
			precision
		);
		int base_flag = wxLEFT;
		if (i == 0) base_flag |= wxTOP;
		m_top_sizer->
			Add(account_name_text, 2, base_flag | wxRIGHT | wxALIGN_LEFT, 10);
		m_top_sizer->
			Add(comment_ctrl, 3, base_flag | wxALIGN_LEFT, 10);
		m_top_sizer->
			Add(entry_ctrl, 2, base_flag | wxRIGHT | wxALIGN_RIGHT, 10);
		m_amount_boxes.push_back(entry_ctrl);
	}

	// Date control
	m_date_ctrl = new wxTextCtrl
	(	this,
		s_date_ctrl_id,
		wxEmptyString,
		wxDefaultPosition,
		m_ok_button->GetSize(),
		wxALIGN_RIGHT,
		DateValidator(today())	
	);

	m_top_sizer->AddStretchSpacer();
	m_top_sizer->
		Add(m_date_ctrl, 2, wxALIGN_RIGHT | wxLEFT | wxTOP, 10);
	m_top_sizer->AddStretchSpacer();

	// Cancel and OK buttons
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxDefaultSize
	);
	m_top_sizer->Add
	(	m_cancel_button,
		1,
		wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM | wxTOP,
		10
	);
	m_top_sizer->AddStretchSpacer();
	m_top_sizer->Add
	(	m_ok_button,
		1,
		wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM | wxTOP,
		10
	);

	// "Admin"
	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
	CentreOnScreen();
}

void
TransactionDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	if (Validate() && TransferDataFromWindow())
	{
		assert (IsModal());
		if (is_balanced())
		{
			// TODO Actually post the transaction or etc.
			EndModal(wxID_OK);
		}
		else
		{
			wxMessageBox("Transaction does not balance.");
		}
	}
	else
	{
		JEWEL_DEBUG_LOG << "Problem validating and/or transferring data from window." << endl;
	}
	return;
}

bool
TransactionDialog::is_balanced() const
{
	Decimal balance(0, 0);
	vector<DecimalTextCtrl*>::size_type i = 0;
	vector<DecimalTextCtrl*>::size_type const sz = m_amount_boxes.size();
	for ( ; i != sz; ++i)
	{
		balance += wx_to_decimal(m_amount_boxes[i]->GetValue(), locale());
	}
	return balance == Decimal(0, 0);
}


}  // namespace gui
}  // namespace phatbooks

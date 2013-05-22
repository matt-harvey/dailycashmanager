// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_dialog.hpp"
#include "account.hpp"
#include "decimal_validator.hpp"
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/calctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
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
	/*
	EVT_BUTTON
	(	wxID_CANCEL,
		TransactionDialog::on_cancel_button_click
	)
	*/
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
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE
	),
	m_top_sizer(0),
	m_calendar_ctrl(0),
	m_ok_button(0),
	m_cancel_button(0)
{
	m_top_sizer = new wxFlexGridSizer(p_accounts.size() + 2, 2, 0, 0);
	vector<Account>::size_type const sz = p_accounts.size();
	vector<Account>::size_type i = 0;
	for ( ; i != sz; ++i)
	{
		wxStaticText* account_name_text = new wxStaticText
		(	this,
			s_start_entry_amount_ids + i,
			bstring_to_wx(p_accounts[i].name()),
			wxDefaultPosition,
			wxDefaultSize,
			wxALIGN_LEFT
		);
		wxTextCtrl* entry_ctrl = new wxTextCtrl
		(	this,
			s_start_entry_amount_ids + i,
			wxEmptyString,
			wxDefaultPosition,
			wxDefaultSize,
			wxALIGN_RIGHT,
			DecimalValidator(Decimal(0, 0))
		);
		m_top_sizer->Add(account_name_text, 1, wxALIGN_LEFT | wxLEFT | wxRIGHT);
		m_top_sizer->Add(entry_ctrl, 1, wxALIGN_RIGHT | wxLEFT | wxRIGHT);
	}
	m_calendar_ctrl = new wxCalendarCtrl(this, wxID_ANY);
	m_top_sizer->AddStretchSpacer();
	m_top_sizer->Add(m_calendar_ctrl);
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxDefaultSize
	);
	m_top_sizer->Add(m_cancel_button);
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&OK"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT
	);
	m_top_sizer->Add(m_ok_button);

	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
	Layout();
}

void
TransactionDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	if (Validate() && TransferDataFromWindow())
	{
		JEWEL_DEBUG_LOG << "Validation and data transfer from window all good." << endl;
	}
	else
	{
		JEWEL_DEBUG_LOG << "Problem validating and/or transferring data from window." << endl;
	}
	return;
}

/*
void
TransactionDialog::on_cancel_button_click(wxCommandEvent& event)
{
	// TODO Is this even necessary?
}
*/


}  // namespace gui
}  // namespace phatbooks

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_dialog.hpp"
#include "account.hpp"
#include "date.hpp"
#include "decimal_text_ctrl.hpp"
#include "decimal_validator.hpp"
#include "entry.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "ordinary_journal.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/datectrl.h>
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

namespace gregorian = boost::gregorian;

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


TransactionDialog::TransactionDialog
(	Frame* p_parent,
	vector<Account> const& p_accounts
):
	wxDialog
	(	p_parent,
		wxID_ANY,
		"New transaction",
		wxDefaultPosition,
		wxDefaultSize
		// , wxRESIZE_BORDER
	),
	m_top_sizer(0),
	m_date_ctrl(0),
	m_cancel_button(0),
	m_ok_button(0),
	m_database_connection(0)
{
	assert (m_account_name_boxes.empty());
	assert (m_comment_boxes.empty());
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
	unsigned int const header_flag = wxLEFT | wxTOP | wxBOTTOM;
	m_top_sizer->Add(header0, 2, header_flag, 10);
	m_top_sizer->Add(header1, 3, header_flag, 10);
	m_top_sizer->Add(header2, 2, header_flag, 10);

	// Rows for entering Entry details
	typedef vector<Account>::size_type Size;
	Size const sz = p_accounts.size();
	for (Size id = s_min_entry_row_id, i = 0 ; i != sz; ++i, ++id)
	{
		Account const account = p_accounts[i];
		if (!m_database_connection)
		{
			m_database_connection = &(account.database_connection());
		}
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

		m_account_name_boxes.push_back(account_name_text);
		m_comment_boxes.push_back(comment_ctrl);
		m_amount_boxes.push_back(entry_ctrl);
	}

	m_top_sizer->AddStretchSpacer();
	m_top_sizer->AddStretchSpacer();

	// Date control
	m_date_ctrl = new wxDatePickerCtrl(this, wxID_ANY);
	m_top_sizer->Add(m_date_ctrl, 2, wxRIGHT | wxLEFT | wxTOP, 10);
	
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
		2,
		wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM | wxTOP,
		10
	);
	m_top_sizer->AddStretchSpacer();
	m_top_sizer->Add
	(	m_ok_button,
		2,
		wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM | wxTOP,
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
	(void)event;  // Silence compiler re. unused parameter.
	if (Validate() && TransferDataFromWindow())
	{
		assert (IsModal());
		if (is_balanced())
		{
			post_journal();
			EndModal(wxID_OK);
		}
		else
		{
			wxMessageBox("Transaction does not balance.");
		}
	}
	return;
}


void
TransactionDialog::post_journal() const
{
	if (m_database_connection)
	{
		OrdinaryJournal journal(*m_database_connection);
		size_t const sz = m_account_name_boxes.size();
		assert (sz == m_comment_boxes.size());
		assert (sz == m_amount_boxes.size());
		for (size_t i = 0; i != sz; ++i)
		{
			assert (m_database_connection);
			Account const account
			(	*m_database_connection,
				wx_to_bstring(wxString(m_account_name_boxes[i]->GetLabel()))
			);
			Entry entry(*m_database_connection);
			entry.set_account(account);
			entry.set_comment
			(	wx_to_bstring(m_comment_boxes[i]->GetValue())
			);
			entry.set_amount
			(	wx_to_decimal
				(	wxString(m_amount_boxes[i]->GetValue()),
					locale()
				)
			);
			entry.set_whether_reconciled(false);
			journal.push_entry(entry);
		}
		assert (journal.is_balanced());
		journal.set_whether_actual(true);
		journal.set_comment("");

		wxDateTime const date_wx = m_date_ctrl->GetValue();
		int year = date_wx.GetYear();
		if (year < 100) year += 2000;
		int const month = static_cast<int>(date_wx.GetMonth()) + 1;
		int const day = date_wx.GetDay();
		journal.set_date(gregorian::date(year, month, day));

		journal.save();

		// TODO Refresh display to refect changes caused by posting
		// journal.
	}
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

#include "budget_dialog.hpp"
#include "account.hpp"
#include "b_string.hpp"
#include "budget_item.hpp"
#include "budget_item_reader.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <vector>

using jewel::Decimal;
using std::vector;

namespace phatbooks
{
namespace gui
{



BudgetDialog::BudgetDialog(Frame* p_parent, Account const& p_account):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_next_row(0),
	m_top_sizer(0),
	m_summary_amount_text(0),
	m_summary_frequency_text(0),
	m_cancel_button(0),
	m_ok_button(0),
	m_account(p_account)
{
	assert (m_account.has_id());  // assert precondition

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	// Row 0
	wxStaticText* account_label = new wxStaticText
	(	this,
		wxID_ANY,
		bstring_to_wx(m_account.name()),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	account_label,
		wxGBPosition(m_next_row, 1),
		wxGBSpan(1, 2),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_summary_amount_text = new wxStaticText
	(	this,
		wxID_ANY,
		generate_summary_amount_text(),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_summary_amount_text,
		wxGBPosition(m_next_row, 3),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_summary_frequency_text = new wxStaticText
	(	this,
		wxID_ANY,
		generate_summary_frequency_text(),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_summary_frequency_text,
		wxGBPosition(m_next_row, 4),
		wxGBSpan(1, 2),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);

	++m_next_row;

	// Row 1
	m_pop_item_button = new wxButton
	(	this,
		s_pop_item_button_id,
		wxString("-"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_pop_item_button,
		wxGBPosition(m_next_row, 4),
		wxDefaultSpan,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	m_push_item_button = new wxButton
	(	this,
		s_push_item_button_id,
		wxString("+"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_push_item_button,
		wxGBPosition(m_next_row, 5),
		wxDefaultSpan,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	
	++m_next_row;

	// Row 2
	// Bare scope
	{
		BudgetItemReader const reader(database_connection());
		BudgetItemReader::const_iterator it = reader.begin();
		BudgetItemReader::const_iterator const end = reader.end();
		for ( ; it != end; ++it)
		{
			 if (it->account() == m_account) push_item(*it);
		}
	}

	++m_next_row;

	// Final row
	
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxDefaultSize
	);
	m_top_sizer->Add(m_cancel_button, wxGBPosition(m_next_row, 1));
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save"),
		wxDefaultPosition,
		wxDefaultSize
	);
	m_top_sizer->
		Add(m_ok_button, wxGBPosition(m_next_row, 4), wxGBSpan(1, 2));
	m_ok_button->SetDefault();  // Enter key will now trigger "Save" button

	// "Admin"
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	Layout();


}

void
BudgetDialog::reset_budget_summary()
{
	// TODO HIGH PRIORITY Implement
}

void
BudgetDialog::push_item(BudgetItem const& p_budget_item)
{
	// TODO HIGH PRIORITY Implement
}

wxString
BudgetDialog::generate_summary_amount_text()
{
	return finformat_wx(m_account.budget(), locale());
}

wxString
BudgetDialog::generate_summary_frequency_text()
{
	return
		std8_to_wx
		(	frequency_description
			(	database_connection().budget_frequency()
			)
		);
}

PhatbooksDatabaseConnection&
BudgetDialog::database_connection()
{
	return m_account.database_connection();
}

}  // namespace gui
}  // namespace phatbooks

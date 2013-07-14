#include "budget_dialog.hpp"
#include "account.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <jewel/decimal.hpp>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>

using jewel::Decimal;

namespace phatbooks
{
namespace gui
{

namespace
{
	

}  // end anonymous namespace


BudgetDialog::BudgetDialog(Frame* p_parent, Account const& p_account):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
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

	int row = 0;

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
		wxGBPosition(row, 1),
		wxDefaultSpan,
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
		wxGBPosition(row, 3),
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
		wxGBPosition(row, 4),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	
	// Row 1

	// TODO Continue implementing
	
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

#include "budget_dialog.hpp"
#include "frame.hpp"
#include "sizing.hpp"
#include <wx/gbsizer.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

BudgetDialog::BudgetDialog(Frame* p_parent, Account const& p_account):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_top_sizer(0),
	m_summary_text(0),
	m_cancel_button(0),
	m_ok_button(0),
	m_account(p_account)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	int row = 0;
}

}  // namespace gui
}  // namespace phatbooks

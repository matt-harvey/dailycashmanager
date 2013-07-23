#include "summary_datum.hpp"
#include <jewel/decimal.hpp>
#include <wx/string.h>

using jewel::Decimal;

namespace phatbooks
{
namespace gui
{

SummaryDatum::SummaryDatum(wxString const& p_label, Decimal const& p_amount):
	m_label(p_label),
	m_amount(p_amount)
{
}

wxString
SummaryDatum::label() const
{
	return m_label;
}

Decimal
SummaryDatum::amount() const
{
	return m_amount;
}

void
SummaryDatum::set_amount(Decimal const& p_amount)
{
	m_amount = p_amount;
	return;
}

}  // namespace gui
}  // namespace phatbooks

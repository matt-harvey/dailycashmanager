#include "summary_datum.hpp"
#include <jewel/decimal.hpp>
#include <wx/string.h>
#include <utility>

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

SummaryDatum::SummaryDatum(SummaryDatum const& rhs):
	m_label(rhs.m_label),
	m_amount(rhs.m_amount)
{
}

SummaryDatum&
SummaryDatum::operator=(SummaryDatum const& rhs)
{
	SummaryDatum temp(rhs);
	if (this != &rhs)
	{
		m_label = rhs.m_label;  // Might throw
		m_amount = rhs.m_amount;  // Can't throw
	}
	return *this;
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

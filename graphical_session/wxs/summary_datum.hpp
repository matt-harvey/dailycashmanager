// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_summary_datum_hpp
#define GUARD_summary_datum_hpp

#include <jewel/decimal.hpp>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

/**
 * Represents a single piece of GUI-displayable financial information,
 * along with a label for that information.
 */
class SummaryDatum
{
public:
	SummaryDatum(wxString const& p_label, jewel::Decimal const& p_amount);
	SummaryDatum(SummaryDatum const& rhs);
	SummaryDatum& operator=(SummaryDatum const& rhs);

	wxString label() const;
	jewel::Decimal amount() const;
	void set_amount(jewel::Decimal const& p_amount);

private:
	wxString m_label;
	jewel::Decimal m_amount;

};  // class SummaryDatum


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_summary_datum_hpp

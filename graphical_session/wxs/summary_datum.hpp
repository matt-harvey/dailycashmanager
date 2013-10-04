// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_summary_datum_hpp_35246402786693726
#define GUARD_summary_datum_hpp_35246402786693726

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
	// synthesised copy constructor, copy assignment and destructor are OK
	// ignore move constructor
	// ignore move assignment
	wxString label() const;
	jewel::Decimal amount() const;
	void set_amount(jewel::Decimal const& p_amount);

private:

	// declare m_label before m_amount to ensure synthesised assignment
	// operator is exception-safe
	wxString m_label;

	jewel::Decimal m_amount;

};  // class SummaryDatum


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_summary_datum_hpp_35246402786693726

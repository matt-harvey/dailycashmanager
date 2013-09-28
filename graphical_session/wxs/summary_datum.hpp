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
	SummaryDatum(SummaryDatum const&) = default;
	SummaryDatum(SummaryDatum&&) = default;
	SummaryDatum& operator=(SummaryDatum const& rhs);
	SummaryDatum& operator=(SummaryDatum&& rhs);
	~SummaryDatum() = default;

	wxString label() const;
	jewel::Decimal amount() const;
	void set_amount(jewel::Decimal const& p_amount);

private:

	wxString m_label;
	jewel::Decimal m_amount;

};  // class SummaryDatum


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_summary_datum_hpp_35246402786693726

#ifndef GUARD_finformat_hpp
#define GUARD_finformat_hpp

#include "b_string.hpp"
#include <wx/intl.h>
#include <string>

namespace jewel
{
	class Decimal;
}


namespace phatbooks
{


/**
 * @returns decimal formatted as a std::string with parentheses to
 * indicate negative, if it's negative.
 *
 * Note the string will have a space added to the end if it's
 * positive. This helps line things up in tables etc..
 * Also if it's zero, it will be converted to the string "-",
 * followed by a number of spaces equal to decimal.places().
 */
std::string finformat_std8(jewel::Decimal const& decimal);

/**
 * @returns decimal formatted as a BString, with parentheses to
 * indicate negative, if it's negative.
 *
 * Note the string will have a space added to the end if it's
 * positive. This helps line things up in tables etc..
 * Also if it's zero, it will be converted to the string "-",
 * followed by a number of spaces equal to decimal.places().
 */
BString finformat_bstring(jewel::Decimal const& decimal);

/**
 * @returns decimal formatted as a wxString, with parentheses
 * to indicate negative, and with thousands separator and
 * decimal point characters drawn from the wxLocale passed to
 * the second parameter, in relation to the wxLocaleCategory
 * wxLOCALE_CAT_MONEY.
 *
 * @todo Testing.
 */
wxString finformat_wx(jewel::Decimal const& decimal, wxLocale const& loc);
	



}  // namespace phatbooks


#endif  // GUARD_finformat_hpp

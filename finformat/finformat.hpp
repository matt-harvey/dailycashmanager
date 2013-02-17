#ifndef GUARD_finformat_hpp
#define GUARD_finformat_hpp

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
 *
 * DO NOT change this to return a wxString. If we want a Decimal
 * formatted as a wxString, create a separate function to do this.
 *
 * @todo Write another function, like finformat_std8, but which returns
 * a wxString.
 */
std::string finformat_std8(jewel::Decimal const& decimal);


}  // namespace phatbooks


#endif  // GUARD_finformat_hpp

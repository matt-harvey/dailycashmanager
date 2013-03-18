#ifndef GUARD_finformat_hpp
#define GUARD_finformat_hpp

#include "b_string.hpp"
#include <string>

namespace jewel
{
	class Decimal;
}


namespace phatbooks
{


// TODO Write a function finformat_to_wxstring(...). Use
// wxWidgets' localization facilities to query the thousands
// separator and decimal point for the current locale, and
// reflect this in the implementation of this function. See
// p. 443 of the wxWidgets book.


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




}  // namespace phatbooks


#endif  // GUARD_finformat_hpp

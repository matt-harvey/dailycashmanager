// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_finformat_hpp
#define GUARD_finformat_hpp

#include "b_string.hpp"
#include <jewel/decimal_fwd.hpp>
#include <wx/intl.h>
#include <string>

namespace phatbooks
{


/**
 * @returns decimal formatted as a std::string with parentheses to
 * indicate negative, if it's negative.
 *
 * Note the string will have an extra space added to the end if it's
 * positive. This helps line things up in tables etc..
 * Also if it's zero, it will be converted to the string "-",
 * followed by a number of spaces equal to decimal.places(), followed
 * again by an additional space to assist with alignment.
 *
 * @param decimal the Decimal to be formatted.
 */
std::string finformat_std8
(	jewel::Decimal const& decimal
);

/**
 * @returns decimal formatted as a BString, with parentheses to
 * indicate negative, if it's negative.
 *
 * Note the string will have an extra space added to the end if it's
 * positive. This helps line things up in tables etc..
 * Also if it's zero, it will be converted to the string "-",
 * followed by a number of spaces equal to decimal.places(), followed
 * again by an additional space to assist with alignment.
 *
 * @param decimal the Decimal to be formatted.
 */
BString finformat_bstring
(	jewel::Decimal const& decimal
);

/**
 * @returns decimal formatted as a wxString, with parentheses
 * to indicate negative, and with thousands separator and
 * decimal point characters drawn from the wxLocale passed to
 * the \e loc parameter, in relation to the wxLocaleCategory
 * wxLOCALE_CAT_MONEY.
 *
 * Note the string will have a space added to the end if it's
 * positive. This helps line things up in tables etc..
 * Also if it's zero, it will be converted to the string "-",
 * followed by a number of spaces equal to decimal.places().
 *
 * If PHATBOOKS_DISALLOW_DASH_FOR_ZERO is defined, then
 * dash is never used for zero, even if \e dash_for_zero is
 * passed \e true.
 *
 * @param decimal the Decimal to be formatted.
 *
 * @param loc the wxLocale according which we want to
 * Decimal to be formatted.
 *
 * @param print zero as '-' character.
 *
 * @todo Testing.
 */
wxString finformat_wx
(	jewel::Decimal const& decimal,
	wxLocale const& loc,
	bool dash_for_zero = true
);


/**
 * Functions named finformat...nopad are like the finformat...
 * functions without "nopad", except they do not add an extra
 * space to the right when formatting non-negative numbers.
 *
 * If PHATBOOKS_DISALLOW_DASH_FOR_ZERO is defined, then
 * dash is never used for zero, even if \e dash_for_zero is
 * passed \e true.

 * @{
 */
std::string finformat_std8_nopad
(	jewel::Decimal const& decimal
);

BString finformat_bstring_nopad
(	jewel::Decimal const& decimal
);

wxString finformat_wx_nopad
(	jewel::Decimal const& decimal,
	wxLocale const& loc,
	bool dash_for_zero = true
);
/** }@ */


/**
 * Assuming a locale of loc, convert a wxString to a jewel::Decimal.
 *
 * This is a fairly tolerant conversion and will accept strings
 * either with or without thousands separators, or with either
 * parentheses or the minus sign as a minus indicator.
 * The symbols that are accepted for the thousands separator and
 * decimal point depend on the wxLocale passed to \e loc.
 *
 * Leading and trailing whitespace is ignored.
 *
 * An empty string will be interpreted as Decimal(0, 0).
 *
 * A string that is just a dash will be interpreted as Decimal(0, 0).
 *
 * If PHATBOOKS_DISALLOW_DASH_FOR_ZERO is defined, then
 * dash is never used for zero, even if \e dash_for_zero is
 * passed \e true.
 */
jewel::Decimal wx_to_decimal(wxString wxs, wxLocale const& loc);


}  // namespace phatbooks


#endif  // GUARD_finformat_hpp

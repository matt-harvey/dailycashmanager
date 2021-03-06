/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 *
 * Utilities for formatting and parsing expressions involving jewel::Decimal.
 */

#ifndef GUARD_finformat_hpp_5275738907640678
#define GUARD_finformat_hpp_5275738907640678

#include "string_flags.hpp"
#include <jewel/decimal_fwd.hpp>
#include <jewel/flag_set.hpp>
#include <wx/intl.h>
#include <wx/string.h>
#include <string>

/**
 * @namespace dcm
 */
namespace dcm
{

/**
 * May be used in some of the below functions to manage boolean
 * options. See \e jewel::FlagSet for interface.
 */
typedef
    jewel::FlagSet
    <   string_flags::StringFlags,
        string_flags::dash_for_zero | string_flags::hard_align_right,
        string_flags::dash_for_zero  // default
    >
    DecimalFormatFlags;

/**
 * May be used in some of the below functions to manage boolean
 * options. See \e jewel::FlagSet for interface.
 */
typedef
    jewel::FlagSet
    <   string_flags::StringFlags,
        string_flags::allow_negative_parens,
        string_flags::allow_negative_parens  // default
    >
    DecimalParsingFlags;

/**
 * @returns decimal formatted as a wxString, with parentheses
 * to indicate negative, and with thousands separator and
 * decimal point characters drawn from the wxLocale passed to
 * the \e loc parameter, in relation to the wxLocaleCategory
 * wxLOCALE_CAT_MONEY.
 *
 * Unless \e p_flags has
 * \e string_flags::hard_align_right set, then for non-negative
 * numbers an extra space will be placed to the right to line
 * things up better in columns.
 *
 * Also if it's zero, it will be converted to the string "-",
 * followed by a number of spaces equal to decimal.places(),
 * followed (unless flagged otherwise per above) by an extra space
 * for alignment. However, zero will be printed "normally" if
 * \e string_flags::dash_for_zero is unset in p_flags.
 *
 * If DCM_DISALLOW_DASH_FOR_ZERO is defined, then
 * dash is never used for zero, regardless of the contents
 * of \e p_flags.
 */
wxString finformat_wx
(   jewel::Decimal const& decimal,
    wxLocale const& loc,
    DecimalFormatFlags p_flags = DecimalFormatFlags()
);

/**
 * Assuming a locale of loc, convert a wxString to a jewel::Decimal.
 *
 * This is a fairly tolerant conversion and will accept strings
 * either with or without thousands separators.
 * The symbols that are accepted for the thousands separator and
 * decimal point depend on the wxLocale passed to \e loc.
 *
 * Leading and trailing whitespace is ignored.
 *
 * An empty string will be interpreted as Decimal(0, 0).
 *
 * A string that is just a dash will be interpreted as Decimal(0, 0).
 *
 * If DCM_DISALLOW_DASH_FOR_ZERO is defined, then
 * dash is never used for zero, regardless of the contents
 * of \e p_flags.
 */
jewel::Decimal wx_to_decimal
(   wxString wxs,
    wxLocale const& loc,
    DecimalParsingFlags p_flags = DecimalParsingFlags()
);

/**
 * This is similar to \e wx_to_decimal, except that: (a) negative must be
 * indicated with a minus sign, not with parentheses, and (b) a simple
 * arithmetic expression will be processed, and the resulting sum
 * returned, providing '+' and '-' are the only operators used, and
 * there are no parentheses.
 */
jewel::Decimal wx_to_simple_sum(wxString wxs, wxLocale const& loc);


}  // namespace dcm


#endif  // GUARD_finformat_hpp_5275738907640678

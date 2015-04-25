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

#include "finformat.hpp"
#include "string_flags.hpp"
#include "string_conv.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/log.hpp>
#include <wx/intl.h>
#include <wx/string.h>
#include <algorithm>
#include <deque>
#include <limits>
#include <locale>
#include <ostream>
#include <string>
#include <vector>

using jewel::Decimal;
using jewel::DecimalFromStringException;
using std::back_inserter;
using std::copy;
using std::deque;
using std::locale;
using std::numeric_limits;
using std::numpunct;
using std::ostringstream;
using std::string;
using std::use_facet;
using std::vector;

namespace dcm
{

namespace
{
    vector<wxString> split_plus_minus(wxString const& p_string)
    {
        vector<wxString> ret;
        wxString current_slice;
        for (wxChar const wx_char: p_string)
        {
            if ((wx_char == wxChar('+')) || (wx_char == wxChar('-')))
            {
                ret.push_back(current_slice);
                current_slice.clear();
            }
            if (wx_char != wxChar(' '))
            {
                current_slice.Append(wx_char);
            }
        }
        ret.push_back(current_slice);
        return ret;
    }

}  // end anonymous namespace

wxString finformat_wx
(   jewel::Decimal const& decimal,
    wxLocale const& loc,
    DecimalFormatFlags p_flags
)
{
#   ifdef DCM_DISALLOW_DASH_FOR_ZERO
        bool const dash_for_zero = false;
#   else
        bool const dash_for_zero = p_flags.test(string_flags::dash_for_zero);
#   endif

    bool const pad = !p_flags.test(string_flags::hard_align_right);

    // TODO LOW PRIORITY Make this cleaner and more efficient.
    Decimal::places_type const places = decimal.places();
    Decimal::int_type const intval = decimal.intval();
    typedef wxChar CharT;
    static CharT const zeroc = wxChar('0');
    wxString const decimal_point_s = loc.GetInfo
    (   wxLOCALE_DECIMAL_POINT,
        wxLOCALE_CAT_MONEY
    );
    wxString const thousands_sep_s = loc.GetInfo
    (   wxLOCALE_THOUSANDS_SEP,
        wxLOCALE_CAT_MONEY
    );
    // We will build it backwards.
    deque<CharT> ret;
    JEWEL_ASSERT (ret.empty());
    // Special case of zero
    if (dash_for_zero && (intval == 0))
    {
        ret.push_back(CharT('-'));
        if (places > 0)
        {
            for (deque<CharT>::size_type i = 0; i != places; ++i)
            {
                ret.push_back(CharT(' '));
            }
        }
    }
    else
    {
        // Our starting point is the string of digits representing the
        // absolute value of the underlying integer.
        ostringstream tempstream;
        tempstream.imbue(locale::classic());
        wxString wxtemp;
        if (intval == numeric_limits<Decimal::int_type>::min())
        {
            // Special case as we can't use std::abs here without
            // overflow.
            tempstream << intval;
            wxtemp = std8_to_wx(tempstream.str());
            wxString::const_iterator it = wxtemp.begin();
            JEWEL_ASSERT (*it == CharT('-'));
            ++it;
            wxtemp = wxString(it, wxtemp.end());
        }
        else
        {
            tempstream << std::abs(intval);
            wxtemp = std8_to_wx(tempstream.str());
        }

        // Write the fractional part
        wxString::reverse_iterator const rend = wxtemp.rend();
        wxString::reverse_iterator rit = wxtemp.rbegin();
        wxString::size_type digits_written = 0;
        for
        (   ;
            (digits_written != places) && (rit != rend);
            ++rit, ++digits_written
        )
        {
            ret.push_front(*rit);
        }
        // Deal with any "filler zerooes" required in the fractional
        // part
        while (digits_written != places)
        {
            ret.push_front(zeroc);
            ++digits_written;
        }
        // Write the decimal point at front if required.
        if (places != 0)
        {
            for (auto k = decimal_point_s.size(); k != 0; --k)
            {
                JEWEL_ASSERT (k >= 1);
                ret.push_front(decimal_point_s[k - 1]);
            }
        }

        // Write the whole part

        // Assume the grouping of digits is normal "threes".
        // TODO MEDIUM PRIORITY Is this a safe assumption? There doesn't seem to
        // be an equivalent of grouping() for wxLocale.
        static vector<wxString::size_type> const grouping(1, 3);
        vector<wxString::size_type>::const_iterator grouping_it =
            grouping.begin();
        vector<wxString::size_type>::const_iterator last_group_datum =
            grouping.end() - 1;
        wxString::size_type digits_written_this_group = 0;
        for
        (   ;
            rit != rend;
            ++rit, ++digits_written, ++digits_written_this_group
        )
        {
            if
            (   digits_written_this_group ==
                static_cast<wxString::size_type>(*grouping_it)
            )
            {
                // Write thousands separator at front.
                for (auto k = thousands_sep_s.size(); k != 0; --k)
                {
                    JEWEL_ASSERT (k >= 1);
                    ret.push_front(thousands_sep_s[k - 1]);
                }
                digits_written_this_group = 0;
                if (grouping_it != last_group_datum) ++grouping_it;
            }
            ret.push_front(*rit);
        }
        // Write a leading zero if required
        if (digits_written == places)
        {
            ret.push_front(zeroc);
        }
    }
    // Indicate negative if required
    if (intval < 0)
    {
        ret.push_front(CharT('('));
        ret.push_back(CharT(')'));
    }
    else if (pad)
    {
        ret.push_back(CharT(' '));
    }
    wxString wret;
    for (CharT const& elem: ret) wret.Append(elem);
    return wret;
}

jewel::Decimal
wx_to_decimal
(   wxString wxs,
    wxLocale const& loc,
    DecimalParsingFlags p_flags
)
{
    bool const allow_parens =
        p_flags.test(string_flags::allow_negative_parens);
    wxs = wxs.Trim().Trim(false);  // trim both right and left.
    typedef wxChar CharT;
    static CharT const open_paren = wxChar('(');
    static CharT const close_paren = wxChar(')');
    static CharT const minus_sign = wxChar('-');
    wxString const decimal_point_s = loc.GetInfo
    (   wxLOCALE_DECIMAL_POINT,
        wxLOCALE_CAT_MONEY
    );
    wxString const thousands_sep_s = loc.GetInfo
    (   wxLOCALE_THOUSANDS_SEP,
        wxLOCALE_CAT_MONEY
    );
    if (wxs.IsEmpty())
    {
        return Decimal(0, 0);
    }
    JEWEL_ASSERT (wxs.Len() >= 1);
    if ((wxs.Len() == 1) && (*(wxs.begin()) == minus_sign))
    {
        return Decimal(0, 0);
    }

    // We first convert wxs into a canonical form in which there are no
    // thousands separators, negativity is indicated only by a minus
    // sign, and the decimal point is '.'.
    if (allow_parens && (wxs[0] == open_paren) && (wxs.Last() == close_paren))
    {
        wxs[0] = minus_sign;  // Replace left parenthesis with minus sign
        wxs.RemoveLast();  // Drop right parenthesis
    }
    wxs.Replace(thousands_sep_s, wxEmptyString);

    // We need to get the std::locale (not wxLocale) related decimal point
    // character, so that we can ensure the Decimal constructor-from-string
    // sees that appropriate decimal point character.
    locale const gloc;  // global locale
    char const spot_char = use_facet<numpunct<char> >(gloc).decimal_point();
    char const spot_str[] = { spot_char, '\0' };
    wxs.Replace(decimal_point_s, wxString(spot_str));

    string const s = wx_to_std8(wxs);
    Decimal const ret(s);
    return ret;
}

Decimal
wx_to_simple_sum(wxString wxs, wxLocale const& loc)
{
    Decimal ret(0, 0);
    auto const flags =
        DecimalParsingFlags().clear(string_flags::allow_negative_parens);
    for (auto const& elem: split_plus_minus(wxs))
    {
        ret += wx_to_decimal(elem, loc, flags);
    }
    return ret;
}

}  // namespace dcm


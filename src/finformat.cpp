/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "finformat.hpp"
#include "string_flags.hpp"
#include "string_conv.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
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
using std::ostringstream;
using std::string;
using std::vector;


// For debugging
#include <jewel/log.hpp>
#include <iostream>
using std::endl;


namespace phatbooks
{

namespace
{
	void split_plus_minus(wxString const& p_string, vector<wxString>& out)
	{
		wxString current_slice;
		for (wxChar const wx_char: p_string)
		{
			if ((wx_char == wxChar('+')) || (wx_char == wxChar('-')))
			{
				out.push_back(current_slice);
				current_slice.clear();
			}
			if (wx_char != wxChar(' '))
			{
				current_slice.Append(wx_char);
			}
		}
		out.push_back(current_slice);
		return;
	}

}  // end anonymous namespace

wxString finformat_wx
(	jewel::Decimal const& decimal,
	wxLocale const& loc,
	DecimalFormatFlags p_flags
)
{
#	ifdef PHATBOOKS_DISALLOW_DASH_FOR_ZERO
		bool const dash_for_zero = false;
#	else
		bool const dash_for_zero = p_flags.test(string_flags::dash_for_zero);
#	endif

	bool const pad = !p_flags.test(string_flags::hard_align_right);

	// TODO LOW PRIORITY Make this cleaner and more efficient.
	Decimal::places_type const places = decimal.places();
	Decimal::int_type const intval = decimal.intval();
	typedef wxChar CharT;
	static CharT const zeroc = wxChar('0');
	wxString const decimal_point_s = loc.GetInfo
	(	wxLOCALE_DECIMAL_POINT,
		wxLOCALE_CAT_MONEY
	);
	wxString const thousands_sep_s = loc.GetInfo
	(	wxLOCALE_THOUSANDS_SEP,
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
			// wxWidgets font alignment hack to make it look good
			// with variable width font.
			ret.push_back(CharT(' '));
			if (pad) ret.push_back(CharT(' '));
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
		(	;
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
		(	;
			rit != rend;
			++rit, ++digits_written, ++digits_written_this_group
		)
		{
			if
			(	digits_written_this_group ==
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
(	wxString wxs,
	wxLocale const& loc,
	DecimalParsingFlags p_flags
)
{
	bool const allow_parens =
		p_flags.test(string_flags::allow_negative_parens);
	wxs = wxs.Trim();
	typedef wxChar CharT;
	static CharT const open_paren = wxChar('(');
	static CharT const close_paren = wxChar(')');
	static CharT const minus_sign = wxChar('-');
	wxString const decimal_point_s = loc.GetInfo
	(	wxLOCALE_DECIMAL_POINT,
		wxLOCALE_CAT_MONEY
	);
	wxString const thousands_sep_s = loc.GetInfo
	(	wxLOCALE_THOUSANDS_SEP,
		wxLOCALE_CAT_MONEY
	);
	if (wxs.IsEmpty())
	{
		// TODO HIGH PRIORITY Should probably throw an exception here, rather
		// than creating a Decimal initialized to zero.
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

	// TODO HIGH PRIORITY This will come unstuck if we have made Decimal
	// constructor-from-string sensitive to locale.
	wxs.Replace(decimal_point_s, wxString("."));

	string s = wx_to_std8(wxs);
	Decimal const ret(s);
	return ret;
}

Decimal
wx_to_simple_sum(wxString wxs, wxLocale const& loc)
{
	vector<wxString> vec;
	split_plus_minus(wxs, vec);
	Decimal total(0, 0);
	vector<wxString>::const_iterator it = vec.begin();
	vector<wxString>::const_iterator const end = vec.end();
	for ( ; it != end; ++it)
	{
		if (!it->IsEmpty())
		{
			total += wx_to_decimal
			(	*it,
				loc,
				DecimalParsingFlags().
					clear(string_flags::allow_negative_parens)
			);
		}
	}
	return total;
}


}  // namespace phatbooks


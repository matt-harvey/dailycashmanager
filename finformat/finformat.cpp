// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "finformat.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <wx/intl.h>
#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>
#include <locale>
#include <ostream>
#include <stdexcept>  // WARNING tmp
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
using std::terminate;  // WARNING tmp
using std::vector;


// For debugging
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


namespace phatbooks
{

namespace
{

	string aux_finformat_std8(Decimal const& decimal, bool pad)
	{
		static Decimal const zero = Decimal(0, 0);
		ostringstream oss;

		// WARNING This doesn't format properly on Windows. It just ignores
		// the thousands separators.
		oss.imbue(locale(""));
		oss << decimal;
		string ret(oss.str());
		if (ret[0] == '-')
		{
			assert (decimal < zero);
			ret[0] = '(';
			ret.push_back(')');
		}
		else if (decimal > zero)
		{
			if (pad) ret.push_back(' ');
		}
		else if (decimal == zero)
		{
			ret = "-" + string(decimal.places() + (pad? 1: 0), ' ');
		}
		else
		{
			assert (false);
		}
		return ret;
	}

	BString aux_finformat_bstring(jewel::Decimal const& decimal, bool pad)
	{
		return std8_to_bstring(aux_finformat_std8(decimal, pad));
	}

	wxString aux_finformat_wx
	(	jewel::Decimal const& decimal,
		wxLocale const& loc,
		bool pad
	)
	{
		// TODO Make this cleaner and more efficient.
		Decimal::places_type const places = decimal.places();
		Decimal::int_type const intval = decimal.intval();
		typedef wxChar CharT;
		CharT const zeroc = wxChar('0');
		wxString const decimal_point_s = loc.GetInfo
		(	wxLOCALE_DECIMAL_POINT,
			wxLOCALE_CAT_MONEY
		);
		wxString const thousands_sep_s = loc.GetInfo
		(	wxLOCALE_THOUSANDS_SEP,
			wxLOCALE_CAT_MONEY
		);
		// TODO Are the following assertions always going to be true? No,
		// they are not...
		assert (decimal_point_s.size() == 1);
		assert (thousands_sep_s.size() == 1);
		CharT const decimal_point = decimal_point_s[0];
		CharT const thousand_sep = thousands_sep_s[0];
		// We will build it backwards.
		deque<CharT> ret;
		assert (ret.empty());
		// Special case of zero
		/*
		if (intval == 0)
		{
			ret.push_back(CharT('-'));
			if (places > 0)
			{
				for (deque<CharT>::size_type i = 0; i != places; ++i)
				{
					ret.push_back(CharT(' '));
				}
				ret.push_back(CharT(' '));  // WARNING wxWidgets font alignment hack to make it look good with variable width font
				if (pad) ret.push_back(CharT(' '));
			}
		}
		else
		*/
		// special case of smallest possible m_intval - as we cannot take the
		// absolute value below
		if (intval == numeric_limits<Decimal::int_type>::min())
		{
			// TODO HIGH PRIORITY Fix this.
			assert (false);
		}
		else
		{
			// Our starting point is the string of digits representing the
			// absolute value of the underlying integer.
			ostringstream tempstream;
			tempstream.imbue(locale::classic());
			tempstream << std::abs(intval);
			wxString const wxtemp =
				bstring_to_wx(std8_to_bstring((tempstream.str())));

			// Write the fractional part
			wxString::const_reverse_iterator const rend = wxtemp.rend();
			wxString::const_reverse_iterator rit = wxtemp.rbegin();
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
			// Write the decimal point if required
			if (places != 0) ret.push_front(decimal_point);

			// Write the whole part

			// Assume the grouping of digits is normal "threes".
			// TODO Is this a safe assumption? There doesn't seem to
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
					ret.push_front(thousand_sep);
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
		for
		(	deque<CharT>::const_iterator dit = ret.begin(), dend = ret.end();
			dit != dend;
			++dit
		)
		{
			wret.Append(*dit);
		}

		return wret;
	}
	
}  // end anonymous namespace




std::string finformat_std8
(	jewel::Decimal const& decimal
)
{
	return aux_finformat_std8(decimal, true);
}

BString finformat_bstring
(	jewel::Decimal const& decimal
)
{
	return aux_finformat_bstring(decimal, true);
}

wxString finformat_wx
(	jewel::Decimal const& decimal,
	wxLocale const& loc
)
{
	return aux_finformat_wx(decimal, loc, true);
}


std::string finformat_std8_nopad
(	jewel::Decimal const& decimal
)
{
	return aux_finformat_std8(decimal, false);
}

BString finformat_bstring_nopad
(	jewel::Decimal const& decimal
)
{
	return aux_finformat_bstring(decimal, false);
}

wxString finformat_wx_nopad
(	jewel::Decimal const& decimal,
	wxLocale const& loc
)
{
	return aux_finformat_wx(decimal, loc, false);
}

jewel::Decimal wx_to_decimal(wxString wxs, wxLocale const& loc)
{
	wxs = wxs.Trim();
	typedef wxChar CharT;
	typedef wxString::size_type sz_t;
	CharT const open_paren = wxChar('(');
	CharT const close_paren = wxChar(')');
	CharT const minus_sign = wxChar('-');
	wxString const decimal_point_s = loc.GetInfo
	(	wxLOCALE_DECIMAL_POINT,
		wxLOCALE_CAT_MONEY
	);
	wxString const thousands_sep_s = loc.GetInfo
	(	wxLOCALE_THOUSANDS_SEP,
		wxLOCALE_CAT_MONEY
	);
	// TODO Are the following assertions always going to be true? No,
	// they are not...
	assert (decimal_point_s.size() == 1);
	assert (thousands_sep_s.size() == 1);
	if (wxs.IsEmpty())
	{
		return Decimal(0, 0);
	}
	assert (wxs.Len() >= 1);
	if (wxs.Len() == 1 && wxs[0] == minus_sign)
	{
		return Decimal(0, 0);
	}

	// We first convert wxs into a canonical form in which there are no
	// thousands separators, negativity is indicated only by a minus
	// sign, and the decimal point is '.'.
	if ( (wxs[0] == open_paren) && (wxs[wxs.Len() - 1] == close_paren) )
	{
		wxs[0] = minus_sign;  // Replace left parenthesis with minus sign
		wxs.RemoveLast();  // Drop right parenthesis
	}
	wxs.Replace(thousands_sep_s, wxEmptyString);

	// TODO This will come unstuck if we ever make Decimal
	// constructor-from-string sensitive to locale.
	wxs.Replace(decimal_point_s, wxString("."));

	string s = wx_to_std8(wxs);
	Decimal const ret(s);
	return ret;

}


}  // namespace phatbooks


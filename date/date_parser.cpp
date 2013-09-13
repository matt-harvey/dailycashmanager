#include "date_parser.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_set.hpp>
#include <jewel/array_utilities.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/string.h>

using boost::optional;
using boost::unordered_set;
using jewel::num_elements;

namespace gregorian = boost::gregorian;

namespace phatbooks
{

namespace
{
	bool is_format_char(char c)
	{
		static char const chars_a[] =
		{	a, A, b, h, B, c, C, d, D, F, g, G, H,
			I, j, m, M, p, r, R, S, T, u, U, V, w,
			W, x, X, y, Y, z, Z
		};
		static unordered_set<char> const chars
		(	jewel::begin(chars_a),
			jewel::end(chars_a)
		);
		return chars.find(a) != chars.end();
	}



}  // end anonymous namespace


DateParser::DateParser
(	wxString const& p_short_format,
	wxString const& p_long_format
):
	m_short_format(p_short_format),
	m_long_format(p_long_format)
{
}

optional<gregorian::date>
DateParser::parse(wxString const& p_string, bool p_be_tolerant) const
{
	optional<gregorian::date> ret;
	wxString::const_iterator parsed_to_position;
	wxDateTime date_wx;
	
	wxString const* formats[] =
	{	&m_short_format,
		&m_long_format
	};
	for (size_t i = 0; i != num_elements(formats); ++i)
	{
		date_wx.ParseFormat
		(	p_string,
			*(formats[i]),
			&parsed_to_position
		);
		if (parsed_to_position == p_string.end())
		{
			// Parsing was successful
			int year = date_wx.GetYear();
			if (year < 100) year += 2000;
			int const month = static_cast<int>(date_wx.GetMonth()) + 1;
			int const day = date_wx.GetDay();
			try
			{
				ret = gregorian::date(year, month, day);
				return ret;
			}
			catch (boost::exception&)
			{
			}
		}
	}
	if (p_be_tolerant)
	{	
		return tolerant_parse(p_string);
	}
	return ret;
}

optional<gregorian::date>
DateParser::tolerant_parse(wxString const& p_string) const
{
	optional<gregorian::date> ret;
	// TODO HIGH PRIORITY Implement this.
	
	

	return ret;
}

}  // namespace phatbooks

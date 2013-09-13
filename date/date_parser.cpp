#include "date_parser.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_set.hpp>
#include <jewel/array_utilities.hpp>
#include <jewel/assert.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <string>
#include <vector>

using boost::algorithm::split;
using boost::optional;
using boost::unordered_set;
using jewel::num_elements;
using std::find;
using std::string;
using std::vector;

namespace gregorian = boost::gregorian;
namespace lambda = boost::lambda;

namespace phatbooks
{

namespace
{
	// Returns true if and only if c is one of the special characters that
	// can appear in a strftime format string representing a field date or
	// time field.
	bool is_date_time_format_char(char c)
	{
		static char const chars_a[] =
		{	'a', 'A', 'b', 'B', 'c', 'D', 'H', 'I', 'j', 'm', 'M', 'P',
			'S', 'U', 'w', 'W', 'x', 'X', 'y', 'Y', 'Z'
		};
		static unordered_set<char> const chars
		(	jewel::begin(chars_a),
			jewel::end(chars_a)
		);
		return chars.find(c) != chars.end();
	}

	// Returns true if and only if c is one of the special characters that
	// can appear in a strftime format string representing a field in a tm
	// struct, which is a date field and not a time field, and which
	// represents in numerical format, either the day of the month, the
	// month, or the year.
	bool is_ordinary_date_format_char(char c)
	{
		static char const chars_a[] =
		{	'D',  // day of the month (01-31)
			'm',  // month (01-12)
			'y',  // year in century (00-99)
			'Y',  // year
		};
		char const* const e = jewel::end(chars_a);
		return find(jewel::begin(chars_a), e, c) != e;
	}

	// Examines a strftime-like date format string. If the string is an
	// "ordinary" date format string, returns an optional initialized with the
	// character that separates the fields in that string; otherwise, returns
	// an uninitialized optional. For this purpose, the string is an
	// "ordinary" date format string if and only if: (a) there are exactly
	// three date fields, being a day-of-month, month, and year field
	// (not necessarily in that order), such that each of these fields is
	// represented by a character for which is_ordinary_date_format_char
	// returns true (along with the usual '%' and possible other modifiers
	// between the '%' and the format char); and (b) there is exactly one
	// separator character, which appears twice in the string, separating
	// the first and second fields, and separating the second and third
	// fields.
	optional<char> separating_char(wxString const& str)
	{
		// Analyse the string; if at any point we find anything inconsistent
		// with "ordinary date format", then we return an uninitialized
		// optional.
		optional<char> separator;
		wxString::const_iterator it = str.begin();
		wxString::const_iterator const end = str.end();
		size_t num_fields = 0;
		while (it != end)
		{
			if (*it == '%')
			{
				// Skip over any "modifiers"
				while ((it != end) && !is_date_time_format_char(*it)) ++it;

				// If we've reached the end without find a
				// date_time_format_char, then it's not ordinary format.
				if (it == end)
				{
					return optional<char>();
				}
				JEWEL_ASSERT (is_date_time_format_char(*it));

				// If our format char is not ordinary, then it's not
				// ordinary format.
				if (!is_ordinary_date_format_char(*it))
				{
					return optional<char>();
				}
				JEWEL_ASSERT (is_ordinary_date_format_char(*it));
				++num_fields;
				++it;

				// Have we found all the fields? Then we should
				// be at the end.
				if (num_fields == 3)
				{
					JEWEL_ASSERT (separator || (it != end));
					return separator;
				}
				JEWEL_ASSERT (num_fields < 3);

				// To be an ordinary format string,
				// the next character should be a separator, and not another
				// format char, and not the end. If we already
				// have encountered a separator, then this should be an
				// instance of the same separator.
				if
				(	(it == end) ||
					(is_date_time_format_char(*it)) ||
					(separator && (*separator != *it))
				)
				{
					return optional<char>();
				}
				separator = *it;
				++it;
			}
			else
			{
				return optional<char>();
			}
		}
		return separator;
	}

	namespace date_component
	{
		enum DateComponent
		{
			day,
			month,
			year
		};
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
	wxString const* formats[] =
	{	&m_short_format,
		&m_long_format
	};
	for (size_t i = 0; i != num_elements(formats); ++i)
	{
		wxString const& format = *(formats[i]);
		optional<char> const maybe_sep = separating_char(format);
		if (!maybe_sep)
		{
			continue;
		}
		JEWEL_ASSERT (maybe_sep);
		char const sep = *maybe_sep;
		vector<wxString> target_fields;
		vector<wxString> format_fields;
		split(target_fields, p_string, (lambda::_1 == sep));
		split(format_fields, p_string, (lambda::_1 == sep));
		if ((target_fields.size() > 3) || (format_fields.size() != 3))
		{
			continue;
		}
		JEWEL_ASSERT (target_fields.size() <= 3);
		JEWEL_ASSERT (format_fields.size() == 3);
		// TODO HIGH PRIORITY...
	}
	return ret;
}

}  // namespace phatbooks

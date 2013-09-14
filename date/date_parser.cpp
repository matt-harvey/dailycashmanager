#include "date_parser.hpp"
#include "date.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <jewel/array_utilities.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

using boost::algorithm::split;
using boost::lexical_cast;
using boost::optional;
using boost::unordered_map;
using boost::unordered_set;
using jewel::Log;
using jewel::num_elements;
using jewel::value;
using std::back_inserter;
using std::find;
using std::string;
using std::transform;
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
		{	'a', 'A', 'b', 'B', 'c', 'd', 'H', 'I', 'j', 'm', 'M', 'P',
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
		{	'd',  // day of the month (01-31)
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
		JEWEL_LOG_TRACE();
		JEWEL_LOG_VALUE(Log::info, str);
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

	enum DateComponentType
	{
		day_component,
		month_component,
		year_component
	};

	optional<DateComponentType> maybe_date_component_type(char c)
	{
		optional<DateComponentType> ret;
		switch (c)
		{
		case 'd':
			ret = day_component;
			break;
		case 'm':
			ret = month_component;
			break;
		case 'y':  // fall through
		case 'Y':
			ret = year_component;
			break;
		}
		return ret;
	}

	// Should only call if we know the last character of s is such as
	// to return an initialized optional when passed to date_component_type.
	DateComponentType date_component_type(wxString const s)
	{
		JEWEL_ASSERT (!s.IsEmpty());
		return value(maybe_date_component_type(static_cast<char>(s.Last())));
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
	// TODO Make this clearer and simpler and better commented.
	// TODO HIGH PRIORITY Test this with some different format strings.
	JEWEL_LOG_TRACE();
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
			JEWEL_LOG_TRACE();
			continue;
		}
		JEWEL_ASSERT (maybe_sep);
		char const sep = *maybe_sep;
		JEWEL_LOG_VALUE(Log::info, sep);
		vector<wxString> target_fields;
		vector<wxString> format_fields;
		split(target_fields, p_string, (lambda::_1 == sep));
		split(format_fields, format, (lambda::_1 == sep));
		if ((target_fields.size() > 3) || (format_fields.size() != 3))
		{
			continue;
		}
		JEWEL_ASSERT (target_fields.size() <= 3);
		JEWEL_ASSERT (format_fields.size() == 3);
#		ifndef NDEBUG
			for (size_t k = 0; k != format_fields.size(); ++k)
			{
				JEWEL_LOG_VALUE(Log::trace, format_fields[k]);
			}
#		endif
		vector<DateComponentType> component_types;
		transform
		(	format_fields.begin(),
			format_fields.end(),
			back_inserter(component_types),
			date_component_type
		);
		JEWEL_ASSERT (component_types.size() == 3);

		unordered_map<DateComponentType, int> components;				

		// bare scope
		{
			vector<wxString>::size_type j = 0;
			vector<wxString>::size_type const szj = target_fields.size();
			for ( ; j != szj; ++j)
			{
				wxString field = target_fields[j];
				if ((*(field.begin()) == '0') && (field.size() > 1))
				{
					field = wxString(field.begin() + 1, field.end());
				}
				int val = -1;
				try
				{
					val = lexical_cast<int>(field);
				}
				catch (boost::bad_lexical_cast&)
				{
					break;
				}
				JEWEL_ASSERT (val >= 0);
				JEWEL_ASSERT (j < 3);
				DateComponentType component_type = component_types[j];
				if (target_fields.size() == 1)
				{
					component_type = day_component;	
				}
				if (target_fields.size() == 2)
				{
					bool day_before_month = false;
					for (size_t n = 0; n != component_types.size(); ++n)
					{
						if (component_types[n] == day_component)
						{
							day_before_month = true;
							break;
						}
						if (component_types[n] == month_component)
						{
							day_before_month = false;
							break;
						}
					}
					if (j == 0)
					{
						component_type =
						(	day_before_month?
							day_component:
							month_component
						);
					}
					else if (j == 1)
					{
						component_type =
						(	day_before_month?
							month_component:
							day_component
						);
					}
				}
				components[component_type] = val;	
			}	
		}  // end bare scope

		if (components.find(month_component) == components.end())
		{
			if (components.find(year_component) != components.end())
			{
				continue;
			}
			components[month_component] = today().month();
		}
		if (components.find(year_component) == components.end())
		{
			components[year_component] = today().year();
		}
		if (components.size() != 3)
		{
			continue;
		}
		if (components.at(year_component) < 100)
		{
			components[year_component] += 2000;
		}
		try
		{
			JEWEL_LOG_TRACE();
			JEWEL_ASSERT (components.size() == 3);
			ret = gregorian::date
			(	components.at(year_component),
				components.at(month_component),
				components.at(day_component)
			);
			break;
		}
		catch (boost::exception&)
		{
			JEWEL_ASSERT (!ret);
			continue;
		}
	}
	return ret;
}

}  // namespace phatbooks

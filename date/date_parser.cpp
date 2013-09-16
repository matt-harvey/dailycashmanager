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
#include <exception>
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
using std::find_first_of;
using std::out_of_range;
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
	// TODO There characters under the C++11 standard in addition to the
	// below.
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

	enum DateComponentType
	{
		day_component,
		month_component,
		year_component
	};

	unordered_map<char, DateComponentType> const&
	ordinary_date_format_chars_map()
	{
		typedef unordered_map<char, DateComponentType> Map;
		static Map ret;
		if (ret.empty())
		{
			ret['d'] = day_component;   // day of month (01-31)
			ret['m'] = month_component; // month (01-12)
			ret['y'] = year_component;  // year in century (00-99)
			ret['Y'] = year_component;  // full year
#			ifndef NDEBUG
				// bare scope
				{
					Map::const_iterator it = ret.begin();
					Map::const_iterator const end = ret.end();
					for ( ; it != end; ++it)
					{
						JEWEL_ASSERT (is_date_time_format_char(it->first));
					}
				}
#			endif  // NDEBUG
		}
		JEWEL_ASSERT (!ret.empty());
		return ret;
	}

	// Returns true if and only if c is one of the special characters that
	// can appear in a strftime format string representing a field in a tm
	// struct, which is a date field and not a time field, and which
	// represents in numerical format, either the day of the month, the
	// month, or the year.
	bool is_ordinary_date_format_char(char c)
	{
		unordered_map<char, DateComponentType> const& map =
			ordinary_date_format_chars_map();
		return map.find(c) != map.end();
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
		JEWEL_ASSERT (static_cast<bool>(ret) == is_ordinary_date_format_char(c));
		return ret;
	}

	// Should only call if we know the last character of s is such as
	// to return an initialized optional when passed to date_component_type.
	DateComponentType date_component_type(wxString const s)
	{
		JEWEL_ASSERT (!s.IsEmpty());
		return value(maybe_date_component_type(static_cast<char>(s.Last())));
	}

	void fill_missing_components(unordered_map<DateComponentType, int>& out)
	{
		switch (out.size())
		{
		case 0:
			out[day_component] = today().day();
			goto fill_month;
		case 1:
			if (out.find(day_component) == out.end()) return;
			fill_month: out[month_component] = today().month();
			goto fill_year;
		case 2:
			if (out.find(month_component) == out.end()) return;
			fill_year: out[year_component] = today().year();
			// fall through
		default:
			return;
		}
	}

	optional<gregorian::date>
	tolerant_parse_aux
	(	wxString const& p_target,
		wxString const& p_format
	)
	{
		optional<char> const maybe_sep = separating_char(p_format);
		if (!maybe_sep)
		{
			return optional<gregorian::date>();
		}
		JEWEL_ASSERT (maybe_sep);
		char const sep = *maybe_sep;
		JEWEL_LOG_VALUE(Log::trace, sep);
		vector<wxString> target_fields;
		vector<wxString> format_fields;
		split(target_fields, p_target, (lambda::_1 == sep));
		split(format_fields, p_format, (lambda::_1 == sep));
		if ((target_fields.size() > 3) || (format_fields.size() != 3))
		{
			return optional<gregorian::date>();
		}
		JEWEL_ASSERT (target_fields.size() <= 3);
		JEWEL_ASSERT (format_fields.size() == 3);
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
					return optional<gregorian::date>();
				}
				JEWEL_ASSERT (val >= 0);
				JEWEL_ASSERT (j < 3);
				DateComponentType component_type = component_types[j];
				if (target_fields.size() == 1)
				{
					component_type = day_component;	
				}
				else if (target_fields.size() == 2)
				{
					DateComponentType const day_and_month[] =
					{	day_component,
						month_component
					};
					DateComponentType const first = *find_first_of
					(	component_types.begin(),
						component_types.end(),
						jewel::begin(day_and_month),
						jewel::end(day_and_month)
					);
					switch (j)
					{
					case 0:
						component_type = first;
						break;
					case 1:
						component_type =
						(	(first == day_component)?
							month_component:
							day_component
						);
						break;
					default:
						JEWEL_HARD_ASSERT (false);
					}
				}
				components[component_type] = val;	
			}	
		}  // end bare scope

		fill_missing_components(components);
		if (components.size() != 3)
		{
			return optional<gregorian::date>();
		}
		if (components.at(year_component) < 100)
		{
			components[year_component] += 2000;
		}
		try
		{
			JEWEL_LOG_TRACE();
			JEWEL_ASSERT (components.size() == 3);
			return optional<gregorian::date>
			(	gregorian::date
				(	components.at(year_component),
					components.at(month_component),
					components.at(day_component)
				)
			);
		}
		catch (out_of_range&)
		{
			return optional<gregorian::date>();
		}
	}

}  // end anonymous namespace


DateParser::DateParser
(	wxString const& p_primary_format,
	wxString const& p_secondary_format
):
	m_primary_format(p_primary_format),
	m_secondary_format(p_secondary_format)
{
}

optional<gregorian::date>
DateParser::parse(wxString const& p_string, bool p_be_tolerant) const
{
	optional<gregorian::date> ret;
	wxString::const_iterator parsed_to_position;
	wxDateTime date_wx;
	
	wxString const* formats[] =
	{	&m_primary_format,
		&m_secondary_format
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
	// TODO HIGH PRIORITY Test this with some different format strings.
	JEWEL_LOG_TRACE();
	optional<gregorian::date> const ret =
		tolerant_parse_aux(p_string, m_primary_format);
	return (ret? ret: tolerant_parse_aux(p_string, m_secondary_format));
}


}  // namespace phatbooks

#ifndef GUARD_date_parser_hpp
#define GUARD_date_parser_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/datetime.h>
#include <wx/string.h>

namespace phatbooks
{

class DateParser
{
public:

	DateParser(wxString const& p_short_format, wxString const& p_long_format);

	/**
	 * If p_be_tolerant is passed \e true, then the DateParser will
	 * be relatively forgiving even if p_string does not strictly comply
	 * with these formats.
	 * 
	 * @returns an optional initialized with the resulting date, or
	 * uninitialized if parsing was unsuccessful.
	 */
	boost::optional<boost::gregorian::date> parse
	(	wxString const& p_string,
		bool p_be_tolerant = false
	) const;

private:

	boost::optional<boost::gregorian::date> tolerant_parse
	(	wxString const& p_string
	) const;

	// Original wxString extracted from wxLocale. But we want to be more
	// tolerant than this.
	wxString const m_short_format;

	// Original wxString extracted from wxLocale. But we want to be more
	// tolerant than this.
	wxString const m_long_format;

};  // class DateParser

}  // namespace phatbooks

#endif  // GUARD_date_parser_hpp

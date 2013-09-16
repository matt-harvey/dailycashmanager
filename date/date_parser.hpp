#ifndef GUARD_date_parser_hpp_5151682568318994
#define GUARD_date_parser_hpp_5151682568318994

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/string.h>

namespace phatbooks
{

class DateParser
{
public:

	/**
	 * Create DateParser with two underlying formats which guide what
	 * strings it will be able successfully to parse. These format
	 * strings are of the type that would be accepted by the std::strftime
	 * function.
	 */
	explicit DateParser
	(	wxString const& p_primary_format =
			wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT),
		wxString const& p_secondary_format =
			wxLocale::GetInfo(wxLOCALE_LONG_DATE_FMT)
	);

	/**
	 * If p_be_tolerant is passed \e true, then the DateParser will
	 * be relatively forgiving even if p_string does not strictly comply
	 * with these formats. Note "tolerant parsing" is significantly slower
	 * than "narrow parsing".
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
	wxString const m_primary_format;

	// Original wxString extracted from wxLocale. But we want to be more
	// tolerant than this.
	wxString const m_secondary_format;

};  // class DateParser

}  // namespace phatbooks

#endif  // GUARD_date_parser_hpp_5151682568318994

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

	DateParser(DateParser const&) = default;
	DateParser(DateParser&&) = default;
	DateParser& operator=(DateParser const&) = default;
	DateParser& operator=(DateParser&&) = default;
	~DateParser() = default;

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

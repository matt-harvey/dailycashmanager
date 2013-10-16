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


#include "date.hpp"
#include "interval_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <wx/string.h>
#include <ostream>

using std::ostream;
namespace gregorian = boost::gregorian;

namespace phatbooks
{

wxString
phrase(IntervalType x, bool is_plural)
{
	wxString const number_marker = is_plural? wxString("s"): wxString("");
	switch (x)
	{
	case IntervalType::days:
		return wxString("day") + number_marker;
	case IntervalType::weeks:
		return wxString("week") + number_marker;
	case IntervalType::months:
		return wxString("month") + number_marker;
	case IntervalType::month_ends:
		return
			wxString("month") +
			number_marker +
			wxString(", on the last day of the month");
	default:
		JEWEL_HARD_ASSERT (false);		
	}
}

bool
is_valid_date_for_interval_type
(	gregorian::date const& p_date,
	IntervalType p_interval_type
)
{
	switch (p_interval_type)
	{
	case IntervalType::days:
		return true;
	case IntervalType::weeks:
		return true;
	case IntervalType::months:
		return p_date.day() <= 28;
	case IntervalType::month_ends:
		return p_date == month_end_for_date(p_date);
	default:
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_HARD_ASSERT (false);
}
		
ostream&
operator<<(ostream& os, IntervalType p_interval_type)
{
	switch (p_interval_type)
	{
	case IntervalType::days:
		os << "days";
		break;
	case IntervalType::weeks:
		os << "weeks";
		break;
	case IntervalType::months:
		os << "months";
		break;
	case IntervalType::month_ends:
		os << "month_ends";
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	return os;
}


}  // namespace phatbooks

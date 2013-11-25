/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "date.hpp"
#include "interval_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <wx/string.h>
#include <ostream>

using std::ostream;
namespace gregorian = boost::gregorian;

namespace dcm
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


}  // namespace dcm

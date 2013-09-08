// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "date.hpp"
#include "interval_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <wx/string.h>

namespace gregorian = boost::gregorian;


namespace phatbooks
{

wxString
phrase(interval_type::IntervalType x, bool is_plural)
{
	wxString const number_marker = is_plural? wxString("s"): wxString("");
	switch (x)
	{
	case interval_type::days:
		return wxString("day") + number_marker;
	case interval_type::weeks:
		return wxString("week") + number_marker;
	case interval_type::months:
		return wxString("month") + number_marker;
	case interval_type::month_ends:
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
	interval_type::IntervalType p_interval_type
)
{
	switch (p_interval_type)
	{
	case interval_type::days:
		return true;
	case interval_type::weeks:
		return true;
	case interval_type::months:
		return p_date.day() <= 28;
	case interval_type::month_ends:
		return p_date == month_end_for_date(p_date);
	default:
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_HARD_ASSERT (false);
}
		


}  // namespace phatbooks

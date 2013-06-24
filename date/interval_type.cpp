// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "b_string.hpp"
#include "interval_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <cassert>

namespace gregorian = boost::gregorian;


namespace phatbooks
{

BString
phrase(interval_type::IntervalType x, bool is_plural)
{
	BString const number_marker = is_plural? BString("s"): BString("");
	switch (x)
	{
	case interval_type::days:
		return BString("day") + number_marker;
	case interval_type::weeks:
		return BString("week") + number_marker;
	case interval_type::months:
		return BString("month") + number_marker;
	case interval_type::month_ends:
		return
			BString("month") +
			number_marker +
			BString(", on the last day of the month");
	default:
		assert (false);		
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
		return (p_date + gregorian::date_duration(1)).day() == 1;
	default:
		assert (false);
	}
	assert (false);
}
		


}  // namespace phatbooks

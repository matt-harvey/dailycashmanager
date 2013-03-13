#include "b_string.hpp"
#include "interval_type.hpp"
#include <cassert>


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



}  // namespace phatbooks

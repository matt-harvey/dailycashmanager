#include "interval_type.hpp"
#include <cassert>
#include <string>

using std::string;

namespace phatbooks
{

string phrase(interval_type::IntervalType x, bool is_plural)
{
	string const number_marker = is_plural? "s": "";
	switch (x)
	{
	case interval_type::days:
		return "day" + number_marker;
	case interval_type::weeks:
		return "week" + number_marker;
	case interval_type::months:
		return "month" + number_marker;
	case interval_type::month_ends:
		return "month" + number_marker + ", on the last day of the month";
	default:
		assert (false);		
	}
}



}  // namespace phatbooks

#include "interval_type.hpp"
#include <cassert>
#include <wx/string.h>


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
		assert (false);		
	}
}



}  // namespace phatbooks

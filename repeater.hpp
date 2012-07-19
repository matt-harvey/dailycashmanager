#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

#include "general_typedefs.hpp"

namespace phatbooks
{

class Repeater
{
private:
	enum IntervalType
	{
		days = 1,
		weeks,
		months,
		month_ends
	};
	IntervalType m_interval_type;
	int m_interval_units;
	DateType m_next_date_due;
};




}  // namespace phatbooks

#endif  // GUARD_repeater_hpp

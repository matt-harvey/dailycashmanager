#ifndef GUARD_date_duration_hpp
#define GUARD_date_duration_hpp

#include "interval_type.hpp"

namespace phatbooks
{

class DateDuration
{
public:
	DateDuration(int p_num_steps, interval_type::IntervalType p_step_type);
	int num_steps() const;
	interval_type::IntervalType step_type() const;

private:
	int m_num_steps;
	interval_type::IntervalType m_step_type;
};



}  // namespace phatbooks


#endif  // GUARD_date_duration_hpp

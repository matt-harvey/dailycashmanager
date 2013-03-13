#include "date_duration.hpp"
#include "interval_type.hpp"

namespace phatbooks
{

DateDuration::DateDuration
(	int p_num_steps,
	interval_type::IntervalType p_step_type
):
	m_num_steps(p_num_steps),
	m_step_type(p_step_type)
{
}

int
DateDuration::num_steps() const
{
	return m_num_steps;
}

int
DateDuration::step_type() const
{
	return m_step_type;
}


}  // namespace phatbooks

#include "frequency.hpp"
#include "interval_type.hpp"

namespace phatbooks
{

Frequency::Frequency
(	int p_num_steps,
	interval_type::IntervalType p_step_type
):
	m_num_steps(p_num_steps),
	m_step_type(p_step_type)
{
}

int
Frequency::num_steps() const
{
	return m_num_steps;
}

int
Frequency::step_type() const
{
	return m_step_type;
}


}  // namespace phatbooks

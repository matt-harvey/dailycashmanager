#include "frequency.hpp"
#include "interval_type.hpp"
#include <boost/lexical_cast.hpp>
#include <string>

using boost::lexical_cast;
using std::string;

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

interval_type::IntervalType
Frequency::step_type() const
{
	return m_step_type;
}

string
frequency_description(Frequency const& frequency)
{
	string ret = "every ";
	int const num_steps = frequency.num_steps();
	if (num_steps > 1)
    {
		ret += lexical_cast<string>(num_steps);
		ret += " ";
		ret += bstring_to_std8(phrase(frequency.step_type(), true));
	}
	else
	{
		ret += bstring_to_std8(phrase(frequency.step_type(), false));
	}
	return ret;
}


}  // namespace phatbooks

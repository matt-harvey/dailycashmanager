#ifndef GUARD_frequency_hpp
#define GUARD_frequency_hpp

#include "interval_type.hpp"
#include <string>

namespace phatbooks
{

class Frequency
{
public:
	Frequency(int p_num_steps, interval_type::IntervalType p_step_type);

	/**
	 * @returns the number of "units" between each event, where the
	 * size of a "unit" is given by step_type().
	 */
	int num_steps() const;

	/**
	 * @returns the IntervalType in which we count the number of "units"
	 * between each event. In other words, this is the "unit of
	 * measure" in which we are measuring the Frequency.
	 */
	interval_type::IntervalType step_type() const;

private:
	int m_num_steps;
	interval_type::IntervalType m_step_type;
};

// Free-standing functions

/**
 * @returns an English adverbial phrase describing the Frequency.
 */
std::string frequency_description(Frequency const& frequency);


}  // namespace phatbooks


#endif  // GUARD_frequency_hpp

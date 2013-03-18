#include "amalgamated_budget.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include <vector>

using std::vector;

namespace phatbooks
{



void
AmalgamatedBudget::generate_supported_frequencies(vector<Frequency>& vec)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supports_frequency. If this
	// changes, that must change too.
	vec.push_back(Frequency(1, interval_type::days));
	vec.push_back(Frequency(1, interval_type::weeks));
	vec.push_back(Frequency(2, interval_type::weeks));
	vec.push_back(Frequency(4, interval_type::weeks));
	vec.push_back(Frequency(1, interval_type::months));
	vec.push_back(Frequency(2, interval_type::months));
	vec.push_back(Frequency(3, interval_type::months));
	vec.push_back(Frequency(4, interval_type::months));
	vec.push_back(Frequency(6, interval_type::months));
	vec.push_back(Frequency(12, interval_type::months));
	return;
}


bool
AmalgamatedBudget::supports_frequency(Frequency const& p_frequency)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supported_frequencies. If this changes,
	// that must change too.
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_frequency.num_steps() == 1;
	case interval_type::weeks:
		switch (p_frequency.num_steps())
		{
		case 1: case 2:	return true;
		case 3:			return false;
		case 4:			return true;
		default: 		return false;
		}
	case interval_type::months:
		switch (p_frequency.num_steps())
		{
		case 1: case 2: case 3: case 4:				return true;
		case 5:										return false;
		case 6:										return true;
		case 7: case 8: case 9: case 10: case 11:	return false;
		case 12:									return true;	
		default:									return false;
		}
	case interval_type::month_ends:
		return false;
	default:
		assert (false);
	}
}
		



}  // namespace phatbooks



#ifndef GUARD_frequency_hpp
#define GUARD_frequency_hpp

#include "interval_type.hpp"
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

/**
 * Represents a recurring period of time, e.g. "every 5 weeks".
 */
class Frequency
{
public:

	/**
	 * @todo int should be positive. Make it throw if it is not
	 * positive. (Should it be an unsigned int? Probably not - this
	 * doesn't rule out zero anyway.) Then write tests to ensure
	 * it throws as expected.
	 */
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
 * @returns a Frequency to be used in Phatbooks as the canonical
 * Frequency for storing certain information. This Frequency is designed
 * to allow for conversion to and from a range of other Frequencies
 * while minimizing rounding loss on "round trip" conversions.
 */
Frequency const canonical_frequency();

/**
 * @returns an English adverbial phrase describing the Frequency.
 */
std::string
frequency_description(Frequency const& frequency);

/**
 * @returns an amount that is equivalent to p_amount, but in
 * annual terms, rather than in terms of p_frequency. E.g. if
 * p_amount is Decimal("10") and p_frequency is
 * Frequency(2, interval_type::month), then 
 * the returned value would be Decimal("60").
 */
jewel::Decimal
convert_to_annual
(	Frequency const& p_frequency,
	jewel::Decimal const& p_amount
);

/**
 * @returns an amount in terms of the frequency
 * given by p_frequency, assuming p_amount is in annual
 * terms. For example, suppose p_amount is Decimal("520"), and
 * p_frequency is Frequency(1, interval_type::week); then
 * the returned value would be Decimal("10").
 */
jewel::Decimal
convert_from_annual
(	Frequency const& p_frequency,
	jewel::Decimal const& p_amount
);

/**
 * @returns an amount that is equivalent to p_amount, but in
 * terms of canonical_frequency(), rather than in terms of p_frequency.
 * <em>C. f.</em> convert_to_annual.
 *
 * @todo Implementation.
 *
 * @todo Testing.
 */
jewel::Decimal
convert_to_canonical
(	Frequency const& p_frequency,
	jewel::Decimal const& p_amount
);

/**
 * @returns an amount in terms of the frequency
 * given by p_frequency, assuming p_amount is in terms of
 * the canonical_frequency().
 * <em>C. f.</em> convert_from_annual.
 *
 * @todo Implementation.
 *
 * @todo Testing.
 */
jewel::Decimal
convert_from_canonical
(	Frequency const& p_frequency,
	jewel::Decimal const& p_amount
);



}  // namespace phatbooks


#endif  // GUARD_frequency_hpp
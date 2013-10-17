/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_frequency_hpp_6254867355457912
#define GUARD_frequency_hpp_6254867355457912

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
	 * @todo HIGH PRIORITY int should be positive. Make it throw if it is not
	 * positive. (Should it be an unsigned int? Probably not - this
	 * doesn't rule out zero anyway. Also remember we need to be sure
	 * it will fit in SQLite 64-bit int type.) Then write tests to ensure
	 * it throws as expected.
	 */
	Frequency(int p_num_steps, IntervalType p_step_type);

	Frequency(Frequency const&) = default;
	Frequency(Frequency&&) = default;
	Frequency& operator=(Frequency const&) = default;
	Frequency& operator=(Frequency&&) = default;
	~Frequency() = default;

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
	IntervalType step_type() const;

private:
	int m_num_steps;
	IntervalType m_step_type;
};

// Free-standing functions

/**
 * @returns a Frequency to be used in Phatbooks as the canonical
 * Frequency for storing certain information. This Frequency is designed
 * to allow for conversion to and from a range of other Frequencies
 * while minimizing rounding loss and the chance of overflow on
 * "round trip" conversions.
 */
Frequency const& canonical_frequency();

/**
 * @returns an English adverbial phrase describing the Frequency.
 * The first word of the description is can be specified (but
 * has a default, seen in function declaration). E.g. we could
 * get either "per 3 weeks" or "every 3 weeks", depending on
 * the string passed to the second parameter.
 */
std::string frequency_description
(	Frequency const& frequency,
	std::string const& first_word = "per"
);

/**
 * @returns an amount that is equivalent to p_amount, but in
 * terms of canonical_frequency(), rather than in terms of p_frequency.
 * <em>C. f.</em> convert_to_annual.
 *
 * @todo HIGH PRIORITY Document circumstances under which this can throw.
 */
jewel::Decimal convert_to_canonical
(	Frequency const& p_frequency,
	jewel::Decimal const& p_amount
);

/**
 * @returns an amount in terms of the frequency
 * given by p_frequency, assuming p_amount is in terms of
 * the canonical_frequency().
 * <em>C. f.</em> convert_from_annual.
 *
 * @todo HIGH PRIORITY Document circumstances under which this can throw.
 */
jewel::Decimal convert_from_canonical
(	Frequency const& p_frequency,
	jewel::Decimal const& p_amount
);

bool operator==(Frequency const& lhs, Frequency const& rhs);

}  // namespace phatbooks


#endif  // GUARD_frequency_hpp_6254867355457912

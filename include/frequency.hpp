/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_frequency_hpp_6254867355457912
#define GUARD_frequency_hpp_6254867355457912

#include "interval_type.hpp"
#include <jewel/decimal.hpp>
#include <string>

namespace dcm
{

/**
 * Represents a recurring period of time, e.g. "every 5 weeks".
 */
class Frequency
{
public:

    /**
     * @throws InvalidFrequencyException if p_num_steps is less than 1
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
 * @returns a Frequency to be used in DCM as the canonical
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
(   Frequency const& frequency,
    std::string const& first_word = "per"
);

/**
 * @returns an amount that is equivalent to p_amount, but in
 * terms of canonical_frequency(), rather than in terms of p_frequency.
 * <em>C. f.</em> convert_to_annual.
 *
 * If \e p_amount is very high, then this might throw either
 * jewel::DecimalMultiplicationException or jewel::DecimalDivisionException.
 */
jewel::Decimal convert_to_canonical
(   Frequency const& p_frequency,
    jewel::Decimal const& p_amount
);

/**
 * @returns an amount in terms of the frequency
 * given by p_frequency, assuming p_amount is in terms of
 * the canonical_frequency().
 * <em>C. f.</em> convert_from_annual.
 *
 * If \e p_amount is very high, then this might throw either
 * jewel::DecimalMultiplicationException or jewel::DecimalDivisionException.
 */
jewel::Decimal convert_from_canonical
(   Frequency const& p_frequency,
    jewel::Decimal const& p_amount
);

/**
 * @returns true if and only if \e lhs and \e rhs are equal both in num_steps()
 * and in step_type(). "Equivalence" is not sufficient for equality. Thus
 * <em>
 * Frequency(7, IntervalType::days) != Frequency(1, IntervalType::weeks)
 * </em>.
 */
bool operator==(Frequency const& lhs, Frequency const& rhs);
bool operator!=(Frequency const& lhs, Frequency const& rhs);

}  // namespace dcm


#endif  // GUARD_frequency_hpp_6254867355457912

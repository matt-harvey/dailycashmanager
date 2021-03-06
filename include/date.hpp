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

#ifndef GUARD_date_hpp_43289257373329837
#define GUARD_date_hpp_43289257373329837

#include "dcm_exceptions.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/string.h>

namespace dcm
{

/**
 * A date is represented as an integral Julian Day. This is for
 * use only as the internal representation of the date, as it
 * simplifies interaction with SQLite, and is compact and efficient.
 * For specialised date manipulations, the boost date library should
 * be preferred.
 *
 * IMPORTANT: This should be a native type. In the class
 * OrdinaryJournal, exception-safe loading (and possibly other
 * aspects of exception-safety) MAY rely on the non-throwing
 * copying behaviour that comes with DateRep being a native type.
 */
typedef int DateRep;

/**
 * Returns \c true iff \c date is a valid
 * value for a date. The earliest date that can be represented
 * is 1 Jan. 1400 CE, represented by DateRep(2232400). Thus
 * numbers lower than this will return false when passed to this
 * function.
 */
bool is_valid_date(DateRep date);

/**
 * Returns a DateType instance that is "null" in value, meaning that
 * it doesn't represent any particular date. This is equivalent
 * to \c DateRep(0).
 */
DateRep null_date_rep();

/**
 * Returns the earliest possible DateRep.
 */
DateRep earliest_date_rep();

/**
 * Returns the latest possible DateRep.
 */
DateRep latest_date_rep();

/**
 * Returns an integer representing the Julian Day representation of a
 * boost::gregorian::date.
 *
 * Returns DateRep(0) if p_date is not a valid date.
 */
DateRep julian_int(boost::gregorian::date p_date);

/**
 * @returns the boost::gregorian::date representation of an integral
 * Julian Day number.
 *
 * Returns boost::gregorian::date(boost::date_time::not_a_date_time)
 * if julian_int is equal to the value returned by null_date().
 *
 * @throws DateConversionException if the returned date would be earlier
 * than 1 Jan. CE. This avoids certain complications.
 */
boost::gregorian::date boost_date_from_julian_int(DateRep julian_int);

/**
 * @returns Today's date, local time.
 */
boost::gregorian::date today();

/**
 * @returns current local time.
 */
boost::posix_time::ptime now();

/**
 * @returns a wxString representation of the p_date, formatted according
 * to the current wxLocale.
 */
wxString date_format_wx(boost::gregorian::date const& p_date);

/**
 * @returns the date that is the last day of the month in which p_date
 * is situated.
 */
boost::gregorian::date month_end_for_date(boost::gregorian::date const& p_date);

}  // namespace dcm


#endif  // GUARD_date_hpp_43289257373329837

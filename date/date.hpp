#ifndef GUARD_date_hpp
#define GUARD_date_hpp

/** \file date.hpp
 *
 * \brief Header file for date-related code for use in Phatbooks
 * application.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "phatbooks_exceptions.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

namespace phatbooks
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
bool
is_valid_date(DateRep date);

/**
 * Returns a DateType instance that is "null" in value, meaning that
 * it doesn't represent any particular date. This is equivalent
 * to \c DateRep(0).
 */
DateRep
null_date_rep();

/**
 * Returns the earliest possible DateRep.
 */
DateRep
earliest_date_rep();

/**
 * Returns the latest possible DateRep.
 */
DateRep
latest_date_rep();

/**
 * Returns an integer representing the Julian Day representation of a
 * boost::gregorian::date.
 *
 * Returns DateRep(0) if p_date is not a valid date.
 */
DateRep
julian_int(boost::gregorian::date p_date);

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
boost::gregorian::date
boost_date_from_julian_int(DateRep julian_int);

	
}  // namespace phatbooks


#endif  // GUARD_date_hpp

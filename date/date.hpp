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


#include "general_typedefs.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

namespace phatbooks
{

/**
 * A date is represented as an integral Julian Day. This is for
 * use only as the internal representation of the date, as it
 * simplifies interaction with SQLite, and is compact and efficient.
 * For specialised date manipulations, the boost date library should
 * be preferred.
 */
typedef int DateRep;

/**
 * Returns \c true iff \c date is a valid
 * value for a date.
 */
bool
is_valid_date(DateRep date);

/**
 * Returns a DateType instance that is "null" in value, meaning that
 * it doesn't represent any particular date.
 */
DateRep
null_date();

/**
 * Returns an integer representing the Julian Day representing of a
 * boost::gregorian::date.
 *
 * @throws std::runtime_error if boost_date is earlier than AD 1582,
 * as the function does not convert such dates accurately.
 */
DateRep
julian_int(boost::gregorian::date p_date);

/**
 * Returns the boost::gregorian::date representation of an integral
 * Julian Day number.
 */
boost::gregorian::date
boost_date_from_julian_int(DateRep julian_int);

	
}  // namespace phatbooks


#endif  // GUARD_date_hpp



/** \file date.cpp
 *
 * \brief Source file containing date-related code for use in Phatbooks
 * application.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */




#include "date.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <stdexcept>

using std::runtime_error;

namespace phatbooks
{

bool
is_valid_date(DateRep date)
{
	return (date > 0);
}

DateRep
null_date()
{
	return DateRep(0);
}


namespace
{
	// Integral Julian representation of 01 Jan. 1970 (12:01am) (POSIX epoch)
	static int const epoch_int = 2440588;
	
	// boost::gregorian representation of POSIX epoch
	static boost::gregorian::date const epoch_grg(1970, 1, 1);
}


DateRep
julian_int(boost::gregorian::date p_date)
{
	boost::gregorian::date_duration const interval = p_date - epoch_grg;
	return epoch_int + interval.days();
}


boost::gregorian::date
boost_date_from_julian_int(DateRep julian_int)
{
	boost::gregorian::date_duration const interval(julian_int - epoch_int);
	return epoch_grg + interval;
}




}  // namespace phatbooks

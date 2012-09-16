

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
#include <cassert>

using std::runtime_error;

namespace phatbooks
{


// Start anonymous namespace
namespace
{
	// Integreal Julian representation of 01 Jan. 1 (12:01am) (CE 1)
	static int const ce_1_int = 1721426;

	// Integral Julian representation of 01 Jan. 1970 (12:01am) (POSIX epoch)
	static int const epoch_int = 2440588;
	
	// boost::gregorian representation of POSIX epoch
	static boost::gregorian::date const epoch_grg(1970, 1, 1);
}
// End anonymous namespace


bool
is_valid_date(DateRep date)
{
	return (date <= ce_1_int);
}

DateRep
null_date_rep()
{
	return DateRep(0);
}


DateRep
julian_int(boost::gregorian::date p_date)
{
	if (p_date.is_not_a_date())
	{
		return null_date_rep();
	}
	if (p_date < boost::gregorian::date(1, 1, 1))
	{
		throw DateConversionException
		(	"julian_int is not designed to handle dates "
			"earlier than year 1 CE."
		);
	}
	boost::gregorian::date_duration const interval = p_date - epoch_grg;
	DateRep const ret = epoch_int + interval.days();
	assert (is_valid_date(ret));
	return ret;
}


boost::gregorian::date
boost_date_from_julian_int(DateRep julian_int)
{
	if (julian_int == null_date_rep())
	{
		// returns an invalid date
		return boost::gregorian::date(boost::date_time::not_a_date_time);
	}
	if (!is_valid_date(julian_int))
	{
		throw DateConversionException
		(	"boost_date_from_julian_int is not designed to handle dates "
			"earlier than year 1 CE."
		);
	}
	boost::gregorian::date_duration const interval(julian_int - epoch_int);
	boost::gregorian::date const ret = epoch_grg + interval;
	assert (ret > boost::gregorian::date(1, 1, 1));
	assert (!ret.is_not_a_date());
	return ret;
}




}  // namespace phatbooks

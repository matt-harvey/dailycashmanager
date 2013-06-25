// Copyright (c) 2013, Matthew Harvey. All rights reserved.



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
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <cassert>
#include <limits>

using std::numeric_limits;

namespace gregorian = boost::gregorian;

namespace phatbooks
{


// Start anonymous namespace
namespace
{
	// Integral Julian representation of 1 Jan 1400 CE (12:01am)
	static int const min_valid_date_rep = 1721426;
	
	// boost::gregorian representation of 1 Jan 1400 CE (12:01am)
	gregorian::date const min_valid_gregorian(1400, 1, 1);

	// Integral Julian representation of 1 Jan 1970 CE (12:01am) (POSIX epoch)
	static int const epoch_date_rep = 2440588;
	
	// boost::gregorian representation of POSIX epoch
	static gregorian::date const epoch_grg(1970, 1, 1);
}
// End anonymous namespace


bool
is_valid_date(DateRep date)
{
	return (date >= min_valid_date_rep);
}

DateRep
null_date_rep()
{
	return DateRep(0);
}


DateRep
earliest_date_rep()
{
	return numeric_limits<DateRep>::min();
}

DateRep
latest_date_rep()
{
	return numeric_limits<DateRep>::max();
}

DateRep
julian_int(gregorian::date p_date)
{
	if (p_date.is_not_a_date())
	{
		return null_date_rep();
	}

	// gregorian::date is never earlier than min_valid_gregorian.
	assert (p_date >= min_valid_gregorian);

	gregorian::date_duration const interval = p_date - epoch_grg;
	DateRep const ret = epoch_date_rep + interval.days();
	assert (is_valid_date(ret));
	return ret;
}


gregorian::date
boost_date_from_julian_int(DateRep julian_int)
{
	if (julian_int == null_date_rep())
	{
		// returns an invalid date
		return gregorian::date(boost::date_time::not_a_date_time);
	}
	if (!is_valid_date(julian_int))
	{
		throw DateConversionException
		(	"boost_date_from_julian_int is not designed to handle dates "
			"earlier than year 1 CE."
		);
	}
	gregorian::date_duration const
		interval(julian_int - epoch_date_rep);
	gregorian::date const ret = epoch_grg + interval;
	assert (ret >= min_valid_gregorian);
	assert (!ret.is_not_a_date());
	return ret;
}


gregorian::date
today()
{
	return gregorian::day_clock::local_day();
}

namespace
{
	wxDateTime boost_to_wx_date(gregorian::date const& p_date)
	{
		assert (static_cast<int>(wxDateTime::Jan) == 0);
		assert (static_cast<int>(wxDateTime::Dec) == 11);
		return wxDateTime
		(	p_date.day(),
			static_cast<wxDateTime::Month>(p_date.month() - 1),
			p_date.year(),
			0,
			0,
			0,
			0
		);
	}


}  // end anonymous namespace


wxString
date_format_wx(gregorian::date const& p_date)
{
	wxDateTime const wxdt = boost_to_wx_date(p_date);
	return wxdt.FormatDate();
}

gregorian::date
month_end_for_date(gregorian::date const& p_date)
{
	int temp_day = 1;
	int temp_month = p_date.month() + 1;
	int temp_year = p_date.year();
	if (temp_month == 13)
	{
		temp_month = 1;
		++temp_year;
		assert (temp_month == 1);
		assert (temp_year == p_date.year() + 1);
	}
	gregorian::date ret(temp_year, temp_month, temp_day);
	ret -= gregorian::date_duration(1);
	assert
	(	( (ret.day() == 28) && (ret.month() == 2) ) ||
		( (ret.day() == 29) && (ret.month() == 2) ) ||
		  (ret.day() == 30) ||
		  (ret.day() == 31)
	);
	assert (ret.year() == p_date.year());
	return ret;
}


}  // namespace phatbooks

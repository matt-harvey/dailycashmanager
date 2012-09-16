
/** \file repeater.cpp
 *
 * \brief Source file pertaining to Repeater class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */




#include "repeater.hpp"
#include "date.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

namespace phatbooks
{

Repeater::Repeater
(	IntervalType p_interval_type,
	int p_interval_units,
	boost::gregorian::date const& p_next_date
):
	m_interval_type(p_interval_type),
	m_interval_units(p_interval_units),
	m_next_date(julian_int(p_next_date))
{
}




}  // namespace phatbooks

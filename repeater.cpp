
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

namespace phatbooks
{

Repeater::Repeater
(	IntervalType p_interval_type,
	int p_interval_units,
	DateType p_next_date
):
	m_draft_journal_id(0),
	m_interval_type(p_interval_type),
	m_interval_units(p_interval_units),
	m_next_date(p_next_date)
{
}




}  // namespace phatbooks

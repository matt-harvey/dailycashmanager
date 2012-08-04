

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

namespace phatbooks
{

bool
is_valid_date(DateType date)
{
	return (date > 0);
}

DateType
null_date()
{
	return DateType(0);
}


}  // namespace phatbooks

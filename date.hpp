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

namespace phatbooks
{

typedef unsigned int DateType;

/**
 * Returns \c true iff \c date is a valid
 * value for a date.
 */
bool is_valid_date(DateType date);

/**
 * Returns a DateType instance that is "null" in value, meaning that
 * it doesn't represent any particular date.
 */
DateType null_date();

}  // namespace phatbooks


#endif  // GUARD_date_hpp

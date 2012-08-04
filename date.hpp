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

bool is_valid_date(DateType date);

DateType null_date();

}  // namespace phatbooks


#endif  // GUARD_date_hpp

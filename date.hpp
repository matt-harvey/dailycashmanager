#ifndef GUARD_date_hpp
#define GUARD_date_hpp

#include "general_typedefs.hpp"

namespace phatbooks
{

typedef unsigned int DateType;

bool is_valid_date(DateType date);

DateType null_date();

}  // namespace phatbooks


#endif  // GUARD_date_hpp

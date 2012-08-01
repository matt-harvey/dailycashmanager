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

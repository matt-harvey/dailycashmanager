#ifndef GUARD_application_hpp
#define GUARD_application_hpp

#include "b_string.hpp"

namespace phatbooks
{

/**
 * Class for encapsulating very general application-level
 * information.
 */
class Application
{
public:
	static BString application_name();
};

}  // namespace phatbooks


#endif  // GUARD_application_hpp

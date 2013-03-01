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

	/**
	 * @returns name of the application as presented to the
	 * user.
	 */
	static BString application_name();

	/**
	 * @returns filename extension to be used with files
	 * belonging to this application. Includes
	 * the '.'.
	 */
	static BString filename_extension();
};

}  // namespace phatbooks


#endif  // GUARD_application_hpp

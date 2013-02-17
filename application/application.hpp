#ifndef GUARD_application_hpp
#define GUARD_application_hpp

#include <wx/string.h>

namespace phatbooks
{

/**
 * Class for encapsulating very general application-level
 * information.
 */
class Application
{
public:
	static wxString application_name();
};

}  // namespace phatbooks


#endif  // GUARD_application_hpp

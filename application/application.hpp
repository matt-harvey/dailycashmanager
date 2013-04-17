#ifndef GUARD_application_hpp
#define GUARD_application_hpp

#include "b_string.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <wx/config.h>

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

	/**
	 * @returns the name of the vendor of the application.
	 */
	static BString vendor_name();

	/**
	 * @returns the filepath of the application file last opened by the
	 * user, stored in a boost::optional. The returned optional
	 * is uninitialized if the user has yet to open an application file,
	 * or if the last opened file cannot be found.
	 */
	static boost::optional<boost::filesystem::path> last_opened_file();

	/**
	 * Record a filepath as being the last application file opened by the
	 * user.
	 *
	 * Precondition: p_path should be an absolute path.
	 */
	static void set_last_opened_file(boost::filesystem::path const& p_path);

private:
	
	static wxConfig& config();

};

}  // namespace phatbooks


#endif  // GUARD_application_hpp

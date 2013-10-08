// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_application_hpp_5927853596363148
#define GUARD_application_hpp_5927853596363148

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <wx/config.h>
#include <wx/string.h>

namespace phatbooks
{

/**
 * Class for encapsulating very general application-level
 * information.
 *
 * @todo Make sure we can't clobber an existing config file.
 */
class Application
{
public:

	Application() = delete;
	Application(Application const&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application const&) = delete;
	Application& operator=(Application&&) = delete;
	~Application() = delete;

	/**
	 * @returns name of the application as presented to the
	 * user.
	 */
	static wxString application_name();

	/**
	 * @returns filename extension to be used with files
	 * belonging to this application. Includes
	 * the '.'.
	 */
	static wxString filename_extension();

	/**
	 * @returns the name of the vendor of the application.
	 */
	static wxString vendor_name();

	/**
	 * @returns the filepath of the application file last opened by the
	 * user, stored in a boost::optional. The returned optional
	 * is uninitialized if the user has yet to open an application file,
	 * or if the last opened file cannot be found.
	 */
	static boost::optional<boost::filesystem::path> last_opened_file();

	/**
	 * @returns the default directory in which application files
	 * should be saved unless specified otherwise by the user. This
	 * would generally correspond to the user's home directory. If
	 * an initialized optional is returned, then the directory is
	 * guaranteed to exist at the time it is returned. If an uninitialized
	 * optional is returned, then an existing default home directory could
	 * not be determined. If initialized, then the value of the optional
	 * will be an absolute filepath.
	 */
	static boost::optional<boost::filesystem::path> default_directory();

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


#endif  // GUARD_application_hpp_5927853596363148

/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


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
 * @todo MEDIUM PRIORITY When we are setting up
 * the config file for the first time, there is a tiny chance that the user's
 * system already has a file or registry entry named exactly
 * the same as the config file/registry entry that we are setting
 * up. If this happens wxWidgets crashes with a failed assertion
 * (even on Release build). This isn't very satisfactory. It would be
 * better to crash than proceed and clobber the file; however,
 * it would be better still if it could adapt to the situation and
 * proceed safely... for example, if the config file is not holding any
 * critical information, then it could ignore it, adopt some reasonable
 * default settings instead, and inform the user that that's what it did.
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
	 * @returns a string indicating the version number.
	 */
	static wxString version();

	/**
	 * @returns filename extension to be used with files
	 * belonging to this application. Includes
	 * the '.'.
	 */
	static wxString filename_extension();

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
	
	/**
	 * @returns the name of the vendor of the application.
	 */
	static wxString vendor_name();

	static wxConfig& config();

};

}  // namespace phatbooks


#endif  // GUARD_application_hpp_5927853596363148

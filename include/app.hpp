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


#ifndef GUARD_app_hpp_19666019230925488
#define GUARD_app_hpp_19666019230925488

#include "phatbooks_database_connection.hpp"
#include "gui/frame.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <wx/app.h>
#include <wx/config.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <memory>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

// end forward declarations


class App: public wxApp
{
public:
	App();
	App(App const&) = delete;
	App(App&&) = delete;
	App& operator=(App const&) = delete;
	App& operator=(App&&) = delete;
	~App() = default;

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

	virtual bool OnInit() override;

	virtual int OnExit() override;

	void set_database_connection
	(	std::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
	);

	/**
	 * Notify session of existing application instance (which could
	 * be either a console or a graphical session), so that just after
	 * the wxWidgets initialization code has run, it can display an
	 * appropriate message box and abort.
	 */
	void notify_existing_application_instance();

	wxLocale const& locale() const;

	PhatbooksDatabaseConnection& database_connection();

private:

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

	static wxConfig& config();

	boost::filesystem::path elicit_existing_filepath();

	std::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
	wxLocale m_locale;
	bool m_existing_application_instance_notified;
};

// Implements App& wxGetApp()
DECLARE_APP(App)

}  // namespace phatbooks

#endif  // GUARD_app_hpp_19666019230925488

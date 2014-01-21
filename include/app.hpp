/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_app_hpp_19666019230925488
#define GUARD_app_hpp_19666019230925488

#include "dcm_database_connection.hpp"
#include "gui/error_reporter.hpp"
#include "gui/frame.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/version_fwd.hpp>
#include <wx/app.h>
#include <wx/config.h>
#include <wx/intl.h>
#include <wx/snglinst.h>
#include <wx/string.h>
#include <memory>

namespace dcm
{

// begin forward declarations

class DcmDatabaseConnection;

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
	 * @returns a Version struct indicating the version.
	 */
	static jewel::Version version();

	static wxString copyright();

	static wxString developer_credits();

	static wxString artist_credits();

	static wxString user_guide_url();

	static wxString brief_license_summary();

	static wxString license_url();

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

	bool OnInit() override;
	int OnRun() override;
	int OnExit() override;

	void set_database_filepath
	(	boost::filesystem::path const& p_database_filepath
	);

	wxLocale const& locale() const;

	void display_help_contents();

	DcmDatabaseConnection& database_connection();

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

	void configure_logging();
	
	void make_backup(boost::filesystem::path const& p_original_filepath);

	static wxConfig& config();

	boost::filesystem::path elicit_existing_filepath();

	bool m_exiting_cleanly;
	wxSingleInstanceChecker* m_single_instance_checker;
	std::unique_ptr<DcmDatabaseConnection> m_database_connection;
	boost::optional<boost::filesystem::path> m_database_filepath;
	boost::optional<boost::filesystem::path> m_backup_filepath;
	gui::ErrorReporter m_error_reporter;
	wxLocale m_locale;
};

// Implements App& wxGetApp()
wxDECLARE_APP(App);

}  // namespace dcm

#endif  // GUARD_app_hpp_19666019230925488

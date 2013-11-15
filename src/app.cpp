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

#include "app.hpp"
#include "backup.hpp"
#include "date.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "string_conv.hpp"
#include "gui/error_reporter.hpp"
#include "gui/frame.hpp"
#include "gui/locale.hpp"
#include "gui/setup_wizard.hpp"
#include "gui/welcome_dialog.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <jewel/version.hpp>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/snglinst.h>
#include <wx/string.h>
#include <wx/tooltip.h>
#include <wx/wx.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

using boost::optional;
using jewel::clear;
using jewel::Log;
using jewel::value;
using jewel::Version;
using std::bad_alloc;
using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::getenv;
using std::ostringstream;
using std::string;
using std::unique_ptr;
using std::vector;
namespace filesystem = boost::filesystem;
namespace posix_time = boost::posix_time;

namespace phatbooks
{

namespace
{
	void flush_standard_output_streams()
	{
		JEWEL_LOG_MESSAGE(Log::info, "Flushing standard output streams.");
		cerr.flush();
		clog.flush();
		cout.flush();
		JEWEL_LOG_MESSAGE(Log::info, "Flushed standard output streams.");
		return;
	}

	bool ensure_dir_exists(string const& p_directory)
	{
		if (filesystem::exists(p_directory))
		{
			return true;
		}
		return filesystem::create_directory(p_directory);
	}

	wxString filepath_wildcard()
	{
		return wxString("*") + App::filename_extension();
	}

	wxString config_location_for_last_opened_file()
	{
		return wxString("/General/LastOpenedFile");
	}

	bool logging_enabled()
	{
#		ifdef JEWEL_ENABLE_LOGGING
			return true;
#		endif
#		ifdef JEWEL_ENABLE_ASSERTION_LOGGING
			return true;
#		endif
#		ifdef JEWEL_ENABLE_EXCEPTION_LOGGING
			return true;
#		endif
		return false;
	}

	wxString vendor_name()
	{
		return wxString("Phatbooks");
	}

	static const wxCmdLineEntryDesc cmd_line_desc[] =
	{	{	wxCMD_LINE_SWITCH,
			"h",
			"help",
			"displays help on the command line parameters",
			wxCMD_LINE_VAL_NONE,
			0
		},
		{	wxCMD_LINE_SWITCH,
			"v",
			"version",
			"print version",
			wxCMD_LINE_VAL_NONE,
			0
		},
		{	wxCMD_LINE_PARAM,
			nullptr,
			nullptr,
			"file to open",
			wxCMD_LINE_VAL_STRING,
			wxCMD_LINE_PARAM_OPTIONAL
		},
		{	wxCMD_LINE_NONE,
			nullptr,
			nullptr,
			nullptr,
			wxCMD_LINE_VAL_NONE,
			0
		}
	};

}  // end anonymous namespace

App::App():
	m_exiting_cleanly(false),
	m_single_instance_checker(nullptr),
	m_database_connection(new PhatbooksDatabaseConnection)
{
	JEWEL_ASSERT (!m_backup_filepath);
}

wxString
App::application_name()
{
	return PHATBOOKS_APPLICATION_NAME;
}

Version
App::version()
{
	return Version
	(	PHATBOOKS_VERSION_MAJOR,
		PHATBOOKS_VERSION_MINOR,
		PHATBOOKS_VERSION_PATCH
	);
}

wxString
App::filename_extension()
{
	return wxString(".phat");
}

optional<filesystem::path>
App::last_opened_file()
{
	optional<filesystem::path> ret;
	wxString wx_path;
	if (config().Read(config_location_for_last_opened_file(), &wx_path))
	{
		string const s_path = wx_to_std8(wx_path);
		JEWEL_ASSERT (!s_path.empty());
		ret = filesystem::path(s_path);
	}
	else
	{
		JEWEL_ASSERT (wx_path.IsEmpty());
		JEWEL_ASSERT (!ret);
	}
	return ret;
}

void
App::set_last_opened_file(filesystem::path const& p_path)
{
	// Assert precondition
	JEWEL_ASSERT (filesystem::absolute(p_path) == p_path);

	string const s_path(p_path.string());
	wxString const wx_path(std8_to_wx(s_path));
	config().Write(config_location_for_last_opened_file(), wx_path);
	config().Flush();
	return;
}

void
App::configure_logging()
{
	if (logging_enabled())
	{
		Log::set_threshold(Log::trace);
#		ifdef JEWEL_ON_WINDOWS
			string const a("C:\\ProgramData\\");
			string const b("Phatbooks\\");
			string const c("logs\\");
			bool ok = ensure_dir_exists(a);
			if (ok) ok = ensure_dir_exists(a + b);
			if (ok) ok = ensure_dir_exists(a + b + c);
			if (!ok)
			{
				cerr << "Could not create log file." << endl;
				return;
			}
			string const log_dir = a + b + c;
#		else
			string const log_dir = "/tmp/";
#		endif  // JEWEL_ON_WINDOWS
		string const log_path =
			log_dir + wx_to_std8(App::application_name()) + ".log";
		Log::set_filepath(log_path);
		m_error_reporter.
			set_log_file_location(filesystem::path(log_path));
	}
	return;
}

optional<filesystem::path>
App::default_directory()
{
	optional<filesystem::path> ret;
#  ifdef JEWEL_ON_WINDOWS
	char const* win_home_drive = getenv("HOMEDRIVE");
	char const* win_home_path = getenv("HOMEPATH");
	if (win_home_drive && win_home_path)
	{
		string const home_str =
			string(win_home_drive) + string(win_home_path);
#  else
	if (char const* home_str = getenv("HOME"))  // assignment deliberate
	{
#  endif
		filesystem::path const home(home_str);
		if (filesystem::exists(filesystem::status(home)))
		{
			ret = filesystem::absolute(home);
		}
	}
	JEWEL_ASSERT (!ret || (filesystem::absolute(value(ret)) == value(ret)));
	return ret;
}

void
App::make_backup(filesystem::path const& p_original_filepath)
{
	JEWEL_LOG_TRACE();
	try
	{
		m_backup_filepath = phatbooks::make_backup
		(	filesystem::absolute(p_original_filepath),
			filesystem::absolute(p_original_filepath.parent_path()),
			"-backup"
		);
	}
	catch (UniqueNameException&)
	{
		// do nothing
	}
	catch (filesystem::filesystem_error&)
	{
		// do nothing
	}
	catch (bad_alloc&)
	{
		// do nothing
	}
	JEWEL_LOG_TRACE();
	return;
}


wxConfig&
App::config()
{
	static wxConfig conf(application_name(), vendor_name());
	return conf;
}

bool App::OnInit()
{
	try
	{
		configure_logging();
		JEWEL_LOG_MESSAGE(Log::info, "Configured logging.");
		wxApp::SetInstance(this);

		// parse command line
		wxString cmd_filename;
		wxCmdLineParser cmd_parser(cmd_line_desc, argc, argv);
		int res = cmd_parser.Parse(false);  // pass false to suppress auto Usage() message

		// check if use asked for command line help
		if ((res == -1) || (res > 0) || cmd_parser.Found("h"))
		{
			cmd_parser.Usage();
			return false;
		}
		// check if user asked for version
		if ((res == -1) || (res > 0) || cmd_parser.Found("v"))
		{
			cout << wx_to_std8(application_name())
				 << " version "
				 << version()
				 << endl;
			return false;
		}
		// check for filename
		if (cmd_parser.GetParamCount() > 0)
		{
			cmd_filename = cmd_parser.GetParam(0);

			// under Windows, when invoking via a file in Explorer, we are
			// passed the short form; so normalize and make the long form
			wxFileName fname(cmd_filename);
			fname.Normalize
			(	wxPATH_NORM_LONG |
				wxPATH_NORM_DOTS |
				wxPATH_NORM_TILDE |
				wxPATH_NORM_ABSOLUTE
			);
			cmd_filename = fname.GetFullPath();
			m_database_filepath = filesystem::absolute
			(	filesystem::path(wx_to_std8(cmd_filename))
			);
		}
		wxString const instance_identifier =
			application_name() + wxString::Format("-%s", wxGetUserId().c_str());
		m_single_instance_checker =
			new wxSingleInstanceChecker(instance_identifier);
		if (m_single_instance_checker->IsAnotherRunning())
		{
			wxLogError(application_name() + wxString(" is already running."));
			return false;
		}

		// Initialize locale
		if (!m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT))
		{
			wxLogError("Could not initialize locale.");
			return false;
		}
		if (m_database_filepath)
		{
			if (filesystem::exists(*m_database_filepath))
			{
				database_connection().open(*m_database_filepath);
			}
			else
			{
				cerr << "File does not exist.\n"
				     << "To create a new file using the GUI, run with no "
					 << "command line arguments."
					 << endl;
				return false;
			}
		}
		while (!database_connection().is_valid())
		{
			// Then the database connection has not been opened.
			// We need to prompt the user either (a) to open an existing
			// file, or (b) to create a new file via the wizard.
			gui::WelcomeDialog welcome_dialog(database_connection());
			if (welcome_dialog.ShowModal() == wxID_OK)
			{
				if (welcome_dialog.user_wants_new_file())
				{
					gui::SetupWizard setup_wizard(database_connection());
					setup_wizard.run();
					m_database_filepath =
						filesystem::absolute(m_database_connection->filepath());
				}
				else
				{
					auto const filepath = elicit_existing_filepath();
					if (filepath.empty())
					{
						return false;
					}
					else
					{
						m_database_filepath = filesystem::absolute(filepath);
						database_connection().open(*m_database_filepath);
					}
				}
			}
			else
			{
				// User has cancelled rather than opening a file.
				return false;
			}
		}
		JEWEL_ASSERT (database_connection().is_valid());
		JEWEL_ASSERT (m_database_filepath);
		JEWEL_ASSERT
		(	*m_database_filepath ==
			filesystem::absolute(*m_database_filepath)
		);
		JEWEL_ASSERT
		(	m_database_connection->filepath() ==
			*m_database_filepath
		);
		JEWEL_ASSERT (filesystem::exists(*m_database_filepath));
		set_last_opened_file(*m_database_filepath);
		m_error_reporter.set_db_file_location(*m_database_filepath);
		make_backup(*m_database_filepath);
		if (m_backup_filepath)
		{
			m_error_reporter.set_backup_db_file_location(*m_backup_filepath);
		}
		database_connection().set_caching_level(5);
		vector<RepeaterFiringResult> const repeater_firing_results =
			update_repeaters(database_connection());
		gui::Frame* frame =
			new gui::Frame(application_name(), database_connection());
		SetTopWindow(frame);
		frame->Show(true);
		wxToolTip::Enable(true);
		frame->report_repeater_firing_results(repeater_firing_results);

		// Start the event loop
		JEWEL_LOG_MESSAGE(Log::info, "Starting wxWidgets event loop.");
		return true;
	}
	catch (std::exception& e)
	{
		m_error_reporter.report(&e);
	}
	// This is necessary to guarantee the stack is fully unwound no
	// matter what exception is thrown - we're not ONLY doing it
	// for the logging and flushing.
	catch (...)
	{
		m_error_reporter.report();
	}
	flush_standard_output_streams();
	return false;
}

wxLocale const&
App::locale() const
{
	return m_locale;
}

void
App::set_database_filepath(filesystem::path const& p_database_filepath)
{
	m_database_filepath = p_database_filepath;
	return;
}

PhatbooksDatabaseConnection&
App::database_connection()
{
	return *m_database_connection;
}

int App::OnRun()
{
	try
	{
		int const ret = wxApp::OnRun();
		if (ret == 0)
		{
			m_exiting_cleanly = true;
		}
		else
		{
			m_exiting_cleanly = false;
		}
		return ret;
	}
	catch (std::exception& e)
	{
		m_error_reporter.report(&e);
	}
	// This is necessary to guarantee the stack is fully unwound no
	// matter what exception is thrown - we're not ONLY doing it
	// for the logging and flushing.
	catch (...)
	{
		m_error_reporter.report();
	}
	flush_standard_output_streams();
	return 1;
}

int App::OnExit()
{
	JEWEL_LOG_TRACE();
	if (m_backup_filepath && m_exiting_cleanly)
	{
		filesystem::remove(*m_backup_filepath);
	}
	delete m_single_instance_checker;
	m_single_instance_checker = nullptr;
	return m_exiting_cleanly? 0: 1;
}

filesystem::path
App::elicit_existing_filepath()
{
	JEWEL_LOG_TRACE();

	filesystem::path ret;
	JEWEL_ASSERT (ret.empty());
	wxString default_dir = wxEmptyString;
	wxString default_filename = wxEmptyString;

	// Set the default path to the last opened file, if there is one
	optional<filesystem::path> const last_opened = last_opened_file();
	if
	(	last_opened &&
		filesystem::exists(filesystem::status(value(last_opened)))
	)
	{
		filesystem::path const fp = value(last_opened);
		default_dir = std8_to_wx(fp.parent_path().string());
		default_filename = std8_to_wx(fp.filename().string());
	}
	wxFileDialog file_dialog
	(	0,
		wxEmptyString,	
		default_dir,
		default_filename,
		filepath_wildcard(),
		wxFD_FILE_MUST_EXIST
	);
	// Show the dialog
	if (file_dialog.ShowModal() == wxID_OK)
	{
		wxString const filepath_wxs = file_dialog.GetPath();
		ret = filesystem::path(wx_to_std8(filepath_wxs));
	}
	else
	{
		JEWEL_LOG_MESSAGE
		(	Log::warning,
			"In App::elicit_existing_filepath, "
				"file_dialog.ShowModal() != wxID_OK."
		);
		JEWEL_ASSERT (ret.empty());
	}
	return ret;
}

}  // namespace phatbooks

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


#include "app.hpp"
#include "date.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "string_conv.hpp"
#include "gui/frame.hpp"
#include "gui/locale.hpp"
#include "gui/setup_wizard.hpp"
#include "gui/welcome_dialog.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/config.h>
#include <wx/filedlg.h>
#include <wx/snglinst.h>
#include <wx/string.h>
#include <wx/tooltip.h>
#include <wx/wx.h>
#include <cstdlib>
#include <memory>
#include <string>

using boost::optional;
using jewel::Log;
using jewel::value;
using std::getenv;
using std::shared_ptr;
using std::string;
namespace filesystem = boost::filesystem;

namespace phatbooks
{

namespace
{
	wxString filepath_wildcard()
	{
		return wxString("*") + App::filename_extension();
	}

	wxString config_location_for_last_opened_file()
	{
		return wxString("/General/LastOpenedFile");
	}

	wxString vendor_name()
	{
		return wxString("Phatbooks");
	}

}  // end anonymous namespace

wxString
App::application_name()
{
	return wxString("Phatbooks");
}

wxString
App::version()
{
	return "1.0.0";
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
	// TODO Create backup file. Then save absolute path to backup file
	// to m_backup_filepath.
}

void
App::destroy_backup()
{
	if (m_backup_filepath)
	{
		filesystem::remove(*m_backup_filepath);
	}
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
	JEWEL_LOG_TRACE();
	wxString const app_name = application_name();
	wxString const instance_identifier =
		app_name + wxString::Format("-%s", wxGetUserId().c_str());
	m_single_instance_checker =
		new wxSingleInstanceChecker(instance_identifier);
	if (m_single_instance_checker->IsAnotherRunning())
	{
		wxLogError(app_name + wxString(" is already running."));
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
		database_connection().open(*m_database_filepath);
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
			}
			else
			{
				filesystem::path const filepath = elicit_existing_filepath();
				if (filepath.empty())
				{
					return false;
				}
				else
				{
					database_connection().open(filepath);
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
	auto const filepath = database_connection().filepath();
	JEWEL_ASSERT (filesystem::absolute(filepath) == filepath);
	set_last_opened_file(filepath);
	make_backup(filepath);
	database_connection().set_caching_level(5);
	update_repeaters(database_connection());
	gui::Frame* frame = new gui::Frame(app_name, database_connection());
	SetTopWindow(frame);
	frame->Show(true);
	wxToolTip::Enable(true);

	// Start the event loop
	JEWEL_LOG_MESSAGE(Log::info, "Starting wxWidgets event loop.");
	return true;
}

App::App(): m_single_instance_checker(nullptr)
{
	JEWEL_ASSERT (!m_backup_filepath);
}

wxLocale const&
App::locale() const
{
	return m_locale;
}

void
App::set_database_connection
(	shared_ptr<PhatbooksDatabaseConnection> p_database_connection
)
{
	JEWEL_LOG_TRACE();
	m_database_connection = p_database_connection;
	return;
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

int App::OnExit()
{
	JEWEL_LOG_TRACE();
	// TODO Should only destroy the backup file if there were no errors.
	// If there are errors, should retain the backup file, and draw the
	// user's attention to the existence of the backup file.
	destroy_backup();
	delete m_single_instance_checker;
	m_single_instance_checker = nullptr;
	return 0;
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

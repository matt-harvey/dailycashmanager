// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "app.hpp"
#include "application.hpp"
#include "date.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "setup_wizard.hpp"
#include "string_conv.hpp"
#include "welcome_dialog.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <wx/filedlg.h>
#include <wx/string.h>
#include <wx/tooltip.h>
#include <wx/wx.h>


using boost::optional;
using boost::shared_ptr;
using jewel::Log;
using jewel::value;
namespace filesystem = boost::filesystem;

namespace phatbooks
{
namespace gui
{

/*
It seems you should almost always arrange things like this:
 wxFrame
 	wxPanel
		a sizer...
			everything else

The wxPanel might seem pointless, but apparently if you don't use
it you get into all kinds of problems.
*/

App::App(): m_existing_application_instance_notified(false)
{
	JEWEL_LOG_TRACE();
}


bool App::OnInit()
{
	JEWEL_LOG_TRACE();

	wxString const app_name = Application::application_name();

	if (m_existing_application_instance_notified)
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
	while (!database_connection().is_valid())
	{
		// Then the database connection has not been opened.
		// We need to prompt the user either (a) to open an existing
		// file, or (b) to create a new file via the wizard.
		WelcomeDialog welcome_dialog(database_connection());
		if (welcome_dialog.ShowModal() == wxID_OK)
		{
			if (welcome_dialog.user_wants_new_file())
			{
				SetupWizard setup_wizard(database_connection());
				setup_wizard.run();
			}
			else
			{
				filesystem::path const filepath = elicit_existing_filepath();
				if (!filepath.empty())
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
	using filesystem::absolute;
	JEWEL_ASSERT
	(	absolute(database_connection().filepath()) ==
		database_connection().filepath()
	);
	Application::set_last_opened_file(database_connection().filepath());

	database_connection().set_caching_level(5);

	update_repeaters(database_connection());

	Frame* frame = new Frame(app_name, database_connection());
	SetTopWindow(frame);
	frame->Show(true);
	wxToolTip::Enable(true);

	// Start the event loop
	JEWEL_LOG_MESSAGE(Log::info, "Starting wxWidgets event loop.");
	return true;
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
App::notify_existing_application_instance()
{
	JEWEL_LOG_TRACE();
	m_existing_application_instance_notified = true;
}

PhatbooksDatabaseConnection&
App::database_connection()
{
	return *m_database_connection;
}

int App::OnExit()
{
	JEWEL_LOG_TRACE();
	// Any cleanup code here.
	return 0;
}


namespace
{
	wxString filepath_wildcard()
	{
		return wxString("*") + Application::filename_extension();
	}

}  // end anonymous namespace



filesystem::path
App::elicit_existing_filepath()
{
	JEWEL_LOG_TRACE();

	filesystem::path ret;
	JEWEL_ASSERT (ret.empty());
	wxString default_directory = wxEmptyString;
	wxString default_filename = wxEmptyString;

	// Set the default path to the last opened file, if there is one
	optional<filesystem::path> const last_opened =
		Application::last_opened_file();
	if
	(	last_opened &&
		filesystem::exists(filesystem::status(value(last_opened)))
	)
	{
		filesystem::path const fp = value(last_opened);
		default_directory = std8_to_wx(fp.parent_path().string());
		default_filename = std8_to_wx(fp.filename().string());
	}
	wxFileDialog file_dialog
	(	0,
		wxEmptyString,	
		default_directory,
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
		// TODO Then what?
	}
	return ret;
}


}  // namespace gui
}  // namespace phatbooks

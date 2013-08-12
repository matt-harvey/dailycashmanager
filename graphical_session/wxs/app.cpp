// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "app.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "date.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "setup_wizard.hpp"
#include "welcome_dialog.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/optional.hpp>
#include <wx/filedlg.h>
#include <wx/wx.h>

// For debugging
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


using boost::optional;
using boost::shared_ptr;
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

App::App():
	m_existing_application_instance_notified(false)
{
}


bool App::OnInit()
{
	wxString const app_name = bstring_to_wx(Application::application_name());

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
	assert (database_connection().is_valid());
	using filesystem::absolute;
	assert
	(	absolute(database_connection().filepath()) ==
		database_connection().filepath()
	);
	Application::set_last_opened_file(database_connection().filepath());

	// TODO HIGH PRIORITY Notify user of autoposted journals.
	update_repeaters(database_connection());

	Frame* frame = new Frame(app_name, database_connection());
	SetTopWindow(frame);
	frame->Show(true);

	// Start the event loop
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
	m_database_connection = p_database_connection;
	return;
}

void
App::notify_existing_application_instance()
{
	m_existing_application_instance_notified = true;
}

PhatbooksDatabaseConnection&
App::database_connection()
{
	return *m_database_connection;
}

int App::OnExit()
{
	// Any cleanup code here.
	return 0;
}


namespace
{
	wxString filepath_wildcard()
	{
		return wxString("*") +
			bstring_to_wx(Application::filename_extension());
	}

}  // end anonymous namespace



filesystem::path
App::elicit_existing_filepath()
{
	filesystem::path ret;
	assert (ret.empty());
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
		default_directory =
			bstring_to_wx(std8_to_bstring(fp.parent_path().string()));
		default_filename =
			bstring_to_wx(std8_to_bstring(fp.filename().string()));
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
		ret = filesystem::path
		(	bstring_to_std8(wx_to_bstring(filepath_wxs))
		);
	}
	else
	{
		// TODO Then what?
	}
	return ret;
}


}  // namespace gui
}  // namespace phatbooks

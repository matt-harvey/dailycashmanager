#include "app.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "phatbooks_database_connection.hpp"
#include "welcome_dialog.hpp"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <wx/filedlg.h>
#include <wx/snglinst.h>
#include <wx/wx.h>


using boost::shared_ptr;

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



bool App::OnInit()
{
	wxString const app_name = bstring_to_wx(Application::application_name());

	// Prevent multiple instances run by the same user
	wxString const app_name_plus_user_id =
		app_name + wxString::Format("-%s", wxGetUserId().c_str());
	m_checker = new wxSingleInstanceChecker(app_name_plus_user_id);
	if (m_checker->IsAnotherRunning())
	{
		wxLogError(app_name + " is already running.");
		return false;
	}
	// Initialize locale
	if (!m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT))
	{
		wxLogError("Could not initialize locale.");
		return false;
	}
	if (!m_database_connection->is_valid())
	{
		// Then the database connection has not been opened.
		// We need to prompt the user either (a) to open an existing
		// file, or (b) to create a new file via the wizard.
		WelcomeDialog welcome_dialog(*m_database_connection);
		if (welcome_dialog.ShowModal() == wxID_OK)
		{
			if (welcome_dialog.user_wants_new_file())
			{
				// TODO Summon SetupWizard
				// ...
			}
			else
			{
				// TODO Cause last-opened filepath to be remembered
				// from session to session.
				m_database_connection->open(elicit_existing_filepath());
			}
		}
		else
		{
			// TODO Then what?
		}
	}
	assert (m_database_connection->is_valid());
	Frame* frame = new Frame(app_name, *m_database_connection);
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

PhatbooksDatabaseConnection&
App::database_connection()
{
	return *m_database_connection;
}

int App::OnExit()
{
	delete m_checker;
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



boost::filesystem::path
App::elicit_existing_filepath()
{
	boost::filesystem::path ret;
	wxFileDialog file_dialog
	(	0,
		wxEmptyString,
		wxEmptyString,
		wxEmptyString,
		filepath_wildcard(),
		wxFD_FILE_MUST_EXIST
	);
	if (file_dialog.ShowModal() == wxID_OK)
	{
		wxString const filepath_wxs = file_dialog.GetPath();
		ret = boost::filesystem::path
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

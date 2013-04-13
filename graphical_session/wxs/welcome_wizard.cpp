#include "welcome_wizard.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <wx/filedlg.h>
#include <wx/wizard.h>
#include <wx/string.h>
#include <cassert>
#include <string>

using std::string;

namespace phatbooks
{
namespace gui
{


/*** WelcomeWizard ***/

WelcomeWizard::WelcomeWizard
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizard
	(	0,
		wxID_ANY,
		wxString("Welcome!"),
		wxBitmap(icon_xpm),  // TODO Put a proper image here
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	),
	m_database_connection(p_database_connection)
{
	assert (!m_database_connection.is_valid());
}


void
WelcomeWizard::run()
{
	WelcomePage* welcome_page = new WelcomePage(this);
	LocaleSetupPage* locale_setup_page = new LocaleSetupPage(this);
	AccountSetupPage* account_setup_page = new AccountSetupPage(this);
	RunWizard(welcome_page);
	m_database_connection.open(welcome_page->elicit_filepath());
	// TODO Implement
}


/*** WelcomePage ***/

WelcomeWizard::WelcomePage::WelcomePage(WelcomeWizard* parent):
	wxWizardPageSimple(parent),
	m_file_dialog(0)
{
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Welcome to ") +
			bstring_to_wx(Application::application_name())
	);
	wxString const filepath_wildcard
	(	bstring_to_wx(Application::application_name()) +
			" files (*.phat)"
	);
	m_file_dialog = new wxFileDialog
	(	this,
		wxString("Select a ") +
			bstring_to_wx(Application::filename_extension()) +
			wxString(" file to open, or enter name of new file:"),
		wxEmptyString,
		wxEmptyString,
		filepath_wildcard
	);
	// TODO If the user cancels the file dialogue, then there is a
	// "corrupted double-linked list" error, and a backtrace and
	// memory map are printed to the console. Not nice.
}

boost::filesystem::path
WelcomeWizard::WelcomePage::elicit_filepath()
{
	assert (m_file_dialog != 0);
	if (m_file_dialog->ShowModal() == wxID_OK)
	{
		wxString const filepath_wxs = m_file_dialog->GetPath();
		string const ret = bstring_to_std8(wx_to_bstring(filepath_wxs));
		return ret;
	}
	// TODO Now what?
}




/*** LocaleSetupPage ***/

WelcomeWizard::LocaleSetupPage::LocaleSetupPage(WelcomeWizard* parent):
	wxWizardPageSimple(parent)
{
	// TODO Implement
}



/*** AccountSetupPage ***/

WelcomeWizard::AccountSetupPage::AccountSetupPage(WelcomeWizard* parent):
	wxWizardPageSimple(parent)
{
	// TODO Implement
}



}  // namespace gui
}  // namesapce phatbooks



#include "setup_wizard.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <wx/filedlg.h>
#include <wx/radiobox.h>
#include <wx/string.h>
#include <wx/textdlg.h>
#include <wx/wizard.h>
#include <cassert>
#include <string>


using std::string;

namespace phatbooks
{
namespace gui
{


namespace
{
	wxString const app_name()
	{
		return bstring_to_wx(Application::application_name());
	}

}  // end anonymous namespace




/*** SetupWizard ***/

SetupWizard::SetupWizard
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizard
	(	0,
		wxID_ANY,
		app_name() + wxString(" Setup Wizard"),
		wxBitmap(icon_xpm),  // TODO Put a proper image here
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	),
	m_database_connection(p_database_connection)
{
	assert (!m_database_connection.is_valid());
}


void
SetupWizard::run()
{
	FilepathPage* filepath_page =
		new FilepathPage(this, m_database_connection);
	LocalizationPage* localization_page =
		new LocalizationPage(this, m_database_connection);
	AccountPage* account_page =
		new AccountPage(this, m_database_connection);
	filepath_page->SetNext(localization_page);
	localization_page->SetNext(account_page);
	RunWizard(filepath_page);
}


/*** SetupWizard::FilepathPage ***/

SetupWizard::FilepathPage::FilepathPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection)
{
	


}





/*** SetupWizard::LocalizationPage ***/

SetupWizard::LocalizationPage::LocalizationPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection)
{
	// TODO Implement
}



/*** AccountPage ***/

SetupWizard::AccountPage::AccountPage
(	SetupWizard* parent,	
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection)
{
	// TODO Implement
}



}  // namespace gui
}  // namesapce phatbooks



#include "welcome_wizard.hpp"
#include "application.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <cassert>
#include <wx/wizard.h>
#include <wx/string.h>

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
	// TODO Implement
}


/*** WelcomePage ***/

WelcomeWizard::WelcomePage::WelcomePage(WelcomeWizard* parent):
	wxWizardPageSimple(parent)
{
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Welcome to ") +
		bstring_to_wx(Application::application_name())
	);
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



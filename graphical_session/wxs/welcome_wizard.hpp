#ifndef GUARD_welcome_wizard_hpp
#define GUARD_welcome_wizard_hpp

#include <wx/wizard.h>
#include <wx/string.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class Frame;

// End forward declarations


/**
 * Represents the wizard that is presented to GUI users when they first
 * start up the application.
 */
class WelcomeWizard:
	public wxWizard
{
public:
	
	/**
	 * Precondition: p_database_connection is not valid (that's the whole
	 * reason we are calling the wizard...).
	 */
	WelcomeWizard(PhatbooksDatabaseConnection& p_database_connection);

	void run();

private:

	PhatbooksDatabaseConnection& m_database_connection;
	
	class WelcomePage: public wxWizardPageSimple
	{
	public:
		WelcomePage(WelcomeWizard* parent);
	};

	class LocaleSetupPage: public wxWizardPageSimple
	{
	public:
		LocaleSetupPage(WelcomeWizard* parent);

	};

	class AccountSetupPage: public wxWizardPageSimple
	{
	public:
		AccountSetupPage(WelcomeWizard* parent);

	};


};  // WelcomeWizard

}  // namespace gui
}  // namesapce phatbooks


#endif  // GUARD_welcome_wizard_hpp

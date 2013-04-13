#ifndef GUARD_welcome_wizard_hpp
#define GUARD_welcome_wizard_hpp

#include <boost/filesystem.hpp>
#include <wx/filedlg.h>
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
	
	/**
	 * This is the first page the user sees when opening the application
	 * without a file selected. This welcomes the user and prompts them to
	 * select a file.
	 */
	class WelcomePage: public wxWizardPageSimple
	{
	public:
		WelcomePage(WelcomeWizard* parent);

		/**
		 * Get a valid filepath from the user (could be a filepath that
		 * doesn't yet exist).
		 */
		boost::filesystem::path elicit_filepath();

	private:
		wxFileDialog* m_file_dialog;
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

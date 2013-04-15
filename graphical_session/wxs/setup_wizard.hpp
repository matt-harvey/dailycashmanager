#ifndef GUARD_setup_wizard_hpp
#define GUARD_setup_wizard_hpp

#include <boost/filesystem.hpp>
#include <wx/filedlg.h>
#include <wx/radiobox.h>
#include <wx/string.h>
#include <wx/wizard.h>

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
class SetupWizard:
	public wxWizard
{
public:
	
	/**
	 * Precondition: p_database_connection is not valid (that's the whole
	 * reason we are calling the wizard...).
	 */
	SetupWizard(PhatbooksDatabaseConnection& p_database_connection);

	void run();

private:

	PhatbooksDatabaseConnection& m_database_connection;
	
	class FilepathPage;
	class LocalizationPage;
	class AccountPage;


};  // SetupWizard



/**
 * This is the first page the user sees when opening the application
 * without a file selected. This welcomes the user and prompts them to
 * select a file.
 */
class SetupWizard::FilepathPage:
	public wxWizardPageSimple
{
public:
	FilepathPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);
	~FilepathPage();
	// virtual void OnWizardPageChanging(wxWizardEvent& event);

private:

	// Should correspond to choices in radio box
	enum RadioBoxChoice
	{	e_create_new_file = 0,
		e_open_existing_file = 1
	};

	wxFileDialog* m_file_dialog;
	wxRadioBox* m_radio_box;
	PhatbooksDatabaseConnection& m_database_connection;
	
	// DECLARE_EVENT_TABLE()

};  // SetupWizard::FilepathPage




class SetupWizard::LocalizationPage:
	public wxWizardPageSimple
{
public:
	LocalizationPage(SetupWizard* parent);

};  // SetupWizard::LocalizationPage



class SetupWizard::AccountPage:
	public wxWizardPageSimple
{
public:
	AccountPage(SetupWizard* parent);

};  // SetupWizard::AccountPage



}  // namespace gui
}  // namesapce phatbooks


#endif  // GUARD_setup_wizard_hpp

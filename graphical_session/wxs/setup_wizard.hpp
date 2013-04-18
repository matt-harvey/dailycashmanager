#ifndef GUARD_setup_wizard_hpp
#define GUARD_setup_wizard_hpp

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
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
class SetupWizard: public wxWizard
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
class SetupWizard::FilepathPage: public wxWizardPageSimple
{
public:
	FilepathPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);

private:

	void on_directory_button_click(wxCommandEvent& event);

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	wxBoxSizer* m_filename_row_sizer;
	wxBoxSizer* m_directory_row_sizer;
	wxTextCtrl* m_filename_ctrl;
	wxTextCtrl* m_directory_ctrl;
	wxButton* m_directory_button;
	boost::optional<boost::filesystem::path> m_selected_directory;

	static int const s_directory_button_id = wxID_HIGHEST + 1;

	DECLARE_EVENT_TABLE()

};  // SetupWizard::FilepathPage




class SetupWizard::LocalizationPage: public wxWizardPageSimple
{
public:
	LocalizationPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);
private:
	PhatbooksDatabaseConnection& m_database_connection;

};  // SetupWizard::LocalizationPage



class SetupWizard::AccountPage: public wxWizardPageSimple
{
public:
	AccountPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);
private:
	PhatbooksDatabaseConnection& m_database_connection;

};  // SetupWizard::AccountPage



}  // namespace gui
}  // namesapce phatbooks


#endif  // GUARD_setup_wizard_hpp

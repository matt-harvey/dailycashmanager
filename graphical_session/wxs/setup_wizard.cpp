#include "setup_wizard.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <wx/filedlg.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
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
	wxString const wx_app_name()
	{
		return bstring_to_wx(Application::application_name());
	}
	wxString const wx_extension()
	{
		return bstring_to_wx(Application::filename_extension());
	}

}  // end anonymous namespace




/*** SetupWizard ***/

SetupWizard::SetupWizard
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizard
	(	0,
		wxID_ANY,
		wx_app_name() + wxString(" Setup Wizard"),
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
	m_database_connection(p_database_connection),
	m_top_sizer(0),
	m_filename_ctrl(0)
{
	m_top_sizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* filename_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Enter name of new file:")
	);
	m_top_sizer->Add(filename_prompt, 0, 5);
	wxString const ext = wx_extension();
	m_filename_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		wxString("MyBudget") + ext,
		wxDefaultPosition,
		wxDefaultSize,
		0,  // style
		wxDefaultValidator  // TODO We need a proper validator here
	);
	m_top_sizer->Add(m_filename_ctrl);	
		
	// TODO Finish implementing
	// ...
			
	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
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



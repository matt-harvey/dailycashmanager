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

/*** SetupWizard ***/

SetupWizard::SetupWizard
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
SetupWizard::run()
{
	FilepathPage* welcome_page = new FilepathPage(this, m_database_connection);
	LocalizationPage* locale_setup_page = new LocalizationPage(this);
	AccountPage* account_setup_page = new AccountPage(this);
	// welcome_page->SetNext(locale_setup_page);
	locale_setup_page->SetNext(account_setup_page);
	RunWizard(welcome_page);
	// TODO Implement
}


/*** SetupWizard::FilepathPage ***/

/*
BEGIN_EVENT_TABLE(SetupWizard::FilepathPage, wxWizardPageSimple)
	EVT_WIZARD_PAGE_CHANGING
	(	wxID_FORWARD,
		SetupWizard::FilepathPage::OnWizardPageChanging
	)
END_EVENT_TABLE()
*/



SetupWizard::FilepathPage::FilepathPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_file_dialog(0),
	m_radio_box(0),
	m_database_connection(p_database_connection)
{
	wxString const app_name = bstring_to_wx(Application::application_name());
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Welcome to ") + app_name
	);
	// TODO Change all this to accord with new design...

	// Should correspond to enum RadioBoxChoice
	wxString const choices[] =
	{	wxString("Create a new ") + app_name + wxString(" file"),
		wxString("Open an existing file")
	};
	m_radio_box = new wxRadioBox
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		WXSIZEOF(choices),
		choices,
		1,
		wxRA_SPECIFY_COLS
	);
	// TODO Call SetSelection?
	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	main_sizer->Add(m_radio_box, 0, wxALL, 5);
	SetSizer(main_sizer);
	main_sizer->Fit(this);
	RadioBoxChoice const selection =
		static_cast<RadioBoxChoice>(m_radio_box->GetSelection());
	if (selection == e_open_existing_file)
	{
		wxString const filepath_wildcard
		(	bstring_to_wx(Application::application_name()) +
				wxString(" files (*") +
				bstring_to_wx(Application::filename_extension()) +
				wxString(")")
		);
		m_file_dialog = new wxFileDialog
		(	this,
			wxString("Select a ") +
				bstring_to_wx(Application::filename_extension()) +
				wxString(" file to open, or enter name of new file:"),
			wxEmptyString,
			wxEmptyString,
			filepath_wildcard,
			wxFD_FILE_MUST_EXIST
		);
		if (m_file_dialog->ShowModal() == wxID_OK)
		{
			wxString const filepath_wxs = m_file_dialog->GetPath();
			string const filepath_std = bstring_to_std8(wx_to_bstring(filepath_wxs));
			m_database_connection.open(filepath_std);
		}
		else
		{
			// TODO Then what?
		}
	}
	else
	{
		assert (selection == e_create_new_file);
		const wxString& new_filename = wxGetTextFromUser
		(	"Enter name of new filepath",
			"",
			"Budget.phat",
			this
		);
		if (new_filename.IsEmpty())
		{
			// TODO Then what?
		}
		// TODO Check extension is OK.
		const wxString& dir = wxDirSelector("Choose a folder");
		if (!dir.empty())
		{
			// TODO Process directory selection
		}	
		string const filepath_base = bstring_to_std8(wx_to_bstring(new_filename));
		string const directory = bstring_to_std8(wx_to_bstring(dir));
		// TODO Finish this
		//
	}
}



SetupWizard::FilepathPage::~FilepathPage()
{
	if (m_file_dialog) m_file_dialog->Destroy();
}



/*** SetupWizard::LocalizationPage ***/

SetupWizard::LocalizationPage::LocalizationPage(SetupWizard* parent):
	wxWizardPageSimple(parent)
{
	// TODO Implement
}



/*** AccountPage ***/

SetupWizard::AccountPage::AccountPage(SetupWizard* parent):
	wxWizardPageSimple(parent)
{
	// TODO Implement
}



}  // namespace gui
}  // namesapce phatbooks



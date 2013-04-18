#include "setup_wizard.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/button.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gbsizer.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/wizard.h>
#include <cassert>
#include <string>

using boost::optional;
using jewel::value;
using std::string;

namespace filesystem = boost::filesystem;

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



BEGIN_EVENT_TABLE(SetupWizard::FilepathPage, wxWizardPageSimple)
	EVT_BUTTON
	(	s_directory_button_id,
		SetupWizard::FilepathPage::on_directory_button_click
	)
END_EVENT_TABLE()



SetupWizard::FilepathPage::FilepathPage
(	SetupWizard* parent,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxWizardPageSimple(parent),
	m_database_connection(p_database_connection),
	m_top_sizer(0),
	m_filename_row_sizer(0),
	m_directory_row_sizer(0),
	m_filename_ctrl(0),
	m_directory_ctrl(0),
	m_directory_button(0)
{
	m_top_sizer = new wxBoxSizer(wxVERTICAL);
	m_filename_row_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_directory_row_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxSize const dlg_unit_size(80, 11);

	// First row
	wxStaticText* filename_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Name for new file:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(filename_prompt);
	
	// Second row
	m_top_sizer->Add(m_filename_row_sizer);
	wxString const ext = wx_extension();
	m_filename_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		wxString("MyBudget"),
		wxDefaultPosition,
		wxDLG_UNIT(this, dlg_unit_size),
		0,  // style
		wxDefaultValidator  // TODO We need a proper validator here
	);
	m_filename_row_sizer->Add(m_filename_ctrl);
	wxStaticText* extension_text = new wxStaticText
	(	this,
		wxID_ANY,
		ext,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_BOTTOM
	);
	m_filename_row_sizer->Add(extension_text);
	
	// Third row
	m_top_sizer->AddSpacer(m_filename_ctrl->GetSize().y);

	// Fourth row
	wxStaticText* directory_prompt = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Folder where file should be saved:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(directory_prompt);

	// Fifth row
	m_top_sizer->Add(m_directory_row_sizer);
	wxString default_directory = wxEmptyString;
	optional<filesystem::path> const maybe_directory =
		Application::default_directory();
	if (maybe_directory)
	{
		default_directory =
			bstring_to_wx(std8_to_bstring(value(maybe_directory).string()));
	}
	m_directory_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		default_directory,
		wxDefaultPosition,
		wxDLG_UNIT(this, dlg_unit_size),
		wxTE_READONLY,  // style
		wxDefaultValidator  // TODO We need a proper validator here
	);
	m_directory_button = new wxButton
	(	this,
		s_directory_button_id,
		wxString("&Browse..."),
		wxDefaultPosition,
		wxSize(wxDefaultSize.x, m_directory_ctrl->GetSize().y)
	);
	m_directory_row_sizer->Add(m_directory_ctrl);
	m_directory_row_sizer->Add(m_directory_button, 0, wxLEFT, 5);

	SetSizer(m_top_sizer);
	m_top_sizer->Fit(this);
}


void
SetupWizard::FilepathPage::on_directory_button_click(wxCommandEvent& event)
{
	wxString default_directory = m_directory_ctrl->GetValue();
	filesystem::path const default_path
	(	bstring_to_std8(wx_to_bstring(default_directory))
	);
	if (!filesystem::exists(filesystem::status(default_path)))
	{
		default_directory = wxEmptyString;
	}
	else
	{
		assert (filesystem::absolute(default_path) == default_path);
	}
	wxDirDialog directory_dialog
	(	this,
		wxEmptyString,
		default_directory,
		wxDD_NEW_DIR_BUTTON
	);
	if (directory_dialog.ShowModal() == wxID_OK)
	{
		wxString const wx_directory = directory_dialog.GetPath();
		m_selected_directory =
			filesystem::path(bstring_to_std8(wx_to_bstring(wx_directory)));
		m_directory_ctrl->ChangeValue(wx_directory);
	}
	(void)event;  // Silence compiler warning about unused parameter.
	return;
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



#include "welcome_dialog.hpp"
#include "application.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <jewel/optional.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <cassert>
#include <cmath>

using jewel::value;
using std::max;

namespace phatbooks
{
namespace gui
{


BEGIN_EVENT_TABLE(WelcomeDialog, wxDialog)
	EVT_BUTTON
	(	s_new_file_button_id,
		WelcomeDialog::on_new_file_button_click
	)
	EVT_BUTTON
	(	s_existing_file_button_id,
		WelcomeDialog::on_existing_file_button_click
	)
END_EVENT_TABLE()


namespace
{
	wxString filepath_wildcard()
	{
		return wxString("*") +
			bstring_to_wx(Application::filename_extension());
	}

}  // end anonymous namespace


WelcomeDialog::WelcomeDialog
(	PhatbooksDatabaseConnection& p_database_connection
):
	wxDialog
	(	0,
		wxID_ANY,
		wxString("Welcome to ") +
			bstring_to_wx(Application::application_name()) +
			wxString("!"),
		wxDefaultPosition,
		wxSize(wxDefaultSize.x * 2, wxDefaultSize.y * 2),
		wxFULL_REPAINT_ON_RESIZE | wxRESIZE_BORDER | wxRESIZE_BORDER
	),
	m_database_connection(p_database_connection),
	m_top_sizer(0),
	m_new_file_button(0),
	m_existing_file_button(0)
{
	m_top_sizer = new wxGridSizer(4, 1, 0, 0);
	SetSizer(m_top_sizer);
	configure_buttons();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	CentreOnScreen();	
}


void
WelcomeDialog::configure_buttons()
{
	wxString const file_descriptor = 
		bstring_to_wx(Application::application_name()) + wxString(" file");
	m_top_sizer->AddStretchSpacer();
	// TODO The below code ensures the buttons are the same width, but is
	// dependent on the relative length of the strings in each button. This
	// sucks; but the better way, viz. to call SetSize(...) to resize
	// whichever is the smallest button after they have been created, doesn't
	// seem to work. SetSize(...) doesn't seem to do anything. Need to find
	// a better way.
	m_existing_file_button = new wxButton
	(	this,
		s_existing_file_button_id,
		wxString("Open an existing ") + file_descriptor
	);
	m_new_file_button = new wxButton
	(	this,
		s_new_file_button_id,
		wxString("Create a new ") + file_descriptor,
		wxDefaultPosition,
		m_existing_file_button->GetSize()
	);
	assert
	(	m_new_file_button->GetSize() ==
		m_existing_file_button->GetSize()
	);
	// Use sizer to position buttons on dialog
	m_top_sizer->Add
	(	m_new_file_button,
		1,
		wxALIGN_CENTER | wxBOTTOM | wxLEFT | wxRIGHT,
		20
	);
	m_top_sizer->Add
	(	m_existing_file_button,
		1,
		wxALIGN_CENTER | wxLEFT | wxRIGHT,
		20
	);
}


boost::filesystem::path
WelcomeDialog::get_filepath() const
{
	return value(m_filepath);
}


void
WelcomeDialog::on_new_file_button_click(wxCommandEvent& event)
{
	// TODO Implement
	// ...

	return;
}


void
WelcomeDialog::on_existing_file_button_click(wxCommandEvent& event)
{
	wxFileDialog* file_dialog = new wxFileDialog
	(	this,
		wxEmptyString,
		wxEmptyString,
		wxEmptyString,
		filepath_wildcard(),
		wxFD_FILE_MUST_EXIST
	);
	if (file_dialog->ShowModal() == wxID_OK)
	{
		wxString const filepath_wxs = file_dialog->GetPath();
		m_filepath = boost::filesystem::path
		(	bstring_to_std8(wx_to_bstring(filepath_wxs))
		);
		EndModal(wxID_OK);
	}
	else
	{
		// TODO Then what?
	}
	return;
}

}  // namespace gui
}  // namespace phatbooks


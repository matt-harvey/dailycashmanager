#include "welcome_dialog.hpp"
#include "application.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <cassert>
#include <cmath>

using std::max;

namespace phatbooks
{
namespace gui
{


BEGIN_EVENT_TABLE(WelcomeDialog, wxDialog)
	EVT_BUTTON
	(	s_existing_file_button_id,
		WelcomeDialog::on_existing_file_button_click
	)
	EVT_BUTTON
	(	s_new_file_button_id,
		WelcomeDialog::on_new_file_button_click
	)
	EVT_BUTTON
	(	wxID_CANCEL,
		WelcomeDialog::on_cancel_button_click
	)
END_EVENT_TABLE()



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
		wxRESIZE_BORDER
	),
	m_database_connection(p_database_connection),
	m_user_wants_new_file(false),
	m_top_sizer(0),
	m_existing_file_button(0),
	m_new_file_button(0),
	m_cancel_button(0)
{
	m_top_sizer = new wxGridSizer(7, 1, 0, 0);
	SetSizer(m_top_sizer);
	configure_buttons();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
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
		wxString("Open an &existing ") + file_descriptor
	);
	m_new_file_button = new wxButton
	(	this,
		s_new_file_button_id,
		wxString("Create a &new ") + file_descriptor,
		wxDefaultPosition,
		m_existing_file_button->GetSize()
	);
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		m_existing_file_button->GetSize()
	);
	assert
	(	m_new_file_button->GetSize() ==
		m_existing_file_button->GetSize()
	);
	assert
	(	m_cancel_button->GetSize() ==
		m_existing_file_button->GetSize()
	);
	// Use sizer to position buttons on dialog
	m_top_sizer->Add
	(	m_existing_file_button,
		1,
		wxALIGN_CENTER | wxLEFT | wxRIGHT,
		20
	);
	m_top_sizer->AddStretchSpacer();
	m_top_sizer->Add
	(	m_new_file_button,
		1,
		wxALIGN_CENTER | wxLEFT | wxRIGHT,
		20
	);
	m_top_sizer->AddStretchSpacer();
	m_top_sizer->Add
	(	m_cancel_button,
		1,
		wxALIGN_CENTER | wxLEFT | wxRIGHT,
		20
	);
	m_top_sizer->AddStretchSpacer();
}


void
WelcomeDialog::on_existing_file_button_click(wxCommandEvent& event)
{
	assert (!m_user_wants_new_file);
	EndModal(wxID_OK);
	(void)event;  // Silence compiler warning about unused parameter.
	return;
}


void
WelcomeDialog::on_new_file_button_click(wxCommandEvent& event)
{
	m_user_wants_new_file = true;
	EndModal(wxID_OK);
	(void)event;  // Silence compiler warning about unused parameter.
	return;
}


void
WelcomeDialog::on_cancel_button_click(wxCommandEvent& event)
{
	assert (!m_user_wants_new_file);
	EndModal(wxID_CANCEL);
	(void)event;  // Silence compiler warning about unused parameter.
	return;
}


bool
WelcomeDialog::user_wants_new_file() const
{
	return m_user_wants_new_file;
}

}  // namespace gui
}  // namespace phatbooks


#include "my_frame.hpp"
#include <wx/string.h>
#include <wx/wx.h>

namespace phatbooks
{
namespace gui
{



MyFrame::MyFrame(wxString const& title):
	wxFrame
	(	0,
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxSize   // Make it full screen (this is a portable way).
		(	wxSystemSettings::GetMetric(wxSYS_SCREEN_X),
			wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)
		)
	)
{
	// TODO The application icon, thus...
	// Set the frame icon
	// SetIcon(wxIcon(my_icon_xpm));

	// Create menus
	wxMenu* helpMenu = new wxMenu;
	wxMenu* fileMenu = new wxMenu;

	// The "About" item should be in the help menu
	helpMenu->Append
	(	wxID_ABOUT,
		wxT("&About...\tF1"),
		wxT("Show about dialog")
	);
	fileMenu->Append
	(	wxID_EXIT,
		wxT("E&xit\tAlt-X"),
		wxT("Quit this program")
	);
	// Now append the freshly created menus to the menu bar...
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(helpMenu, wxT("&Help"));

	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);
}
	

void
MyFrame::OnAbout(wxCommandEvent& event)
{
	// TODO Put proper messages here
	wxString msg;
	msg.Printf("Hello and welcome to %s", wxVERSION_STRING);
	wxMessageBox(msg, "About Minimal", wxOK | wxICON_INFORMATION, this);
	return;
}

void
MyFrame::OnQuit(wxCommandEvent& event)
{
	// Destroy the frame
	Close();
	return;
}







}  // namespace gui
}  // namespace phatbooks

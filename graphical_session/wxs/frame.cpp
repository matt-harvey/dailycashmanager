#include "frame.hpp"
#include "icon.xpm"
#include <jewel/on_windows.hpp>
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/wx.h>

namespace phatbooks
{
namespace gui
{


Frame::Frame(wxString const& title):
	wxFrame
	(	0,
		wxID_ANY,
		title,
		wxDefaultPosition,
#		if JEWEL_ON_WINDOWS
			wxDefaultSize
#		else
			wxSize
			(	wxSystemSettings::GetMetric(wxSYS_SCREEN_X),
				wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)
			)
#		endif
	)
{
	// Set the frame icon
	// TODO I should use SetIcons to associate several icons of
	// different sizes with the Window. This avoids possible ugliness from
	// resizing of a single icon when different sizes are required in
	// different situations.
	SetIcon(wxIcon(icon_xpm));

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

#	if JEWEL_ON_WINDOWS
		Maximize();
#	endif

	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);
}
	

void
Frame::OnAbout(wxCommandEvent& event)
{
	// TODO Put proper messages here
	wxString msg;
	msg.Printf("Hello and welcome to %s", wxVERSION_STRING);
	wxMessageBox(msg, "About Minimal", wxOK | wxICON_INFORMATION, this);
	return;
}

void
Frame::OnQuit(wxCommandEvent& event)
{
	// Destroy the frame
	Close();
	return;
}



}  // namespace gui
}  // namespace phatbooks

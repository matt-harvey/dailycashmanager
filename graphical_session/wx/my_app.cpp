#include "my_app.hpp"
#include "application.hpp"

namespace phatbooks
{
namespace gui
{

bool MyApp::OnInit()
{
	char const* app_name_c = Application::application_name().c_str();
	wxString const app_name(wxString::FromAscii(app_name_c));
	MyFrame* frame = new MyFrame(app_name);
	frame->Connect
	(	wxID_EXIT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(MyFrame::OnQuit)
	);
	frame->Connect
	(	wxID_ABOUT,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(MyFrame::OnAbout)
	);

	// Show it
	frame->Show(true);

	// Start the event loop
	return true;
}



}  // namespace gui
}  // namespace phatbooks

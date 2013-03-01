#include "my_app.hpp"
#include "application.hpp"

#include "b_string.hpp"
#include <wx/wx.h>

namespace phatbooks
{
namespace gui
{

/*
It seems you should almost always arrange things like this:
 wxFrame
 	wxPanel
		a sizer...
			everything else

The wxPanel might seem pointless, but apparently if you don't use
it you get into all kinds of problems.
*/

bool MyApp::OnInit()
{
	wxString const app_name = bstring_to_wx(Application::application_name());

	// Note it breaks if we use scoped_ptr here. We don't need to
	// memory-manage this explicitly anyway, as Destroy() will be called
	// when the user closes the window.
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

	// Child windows are memory-managed by their parents.
	wxPanel* top_panel
	(	new wxPanel
		(	frame,
			wxID_ANY,
			wxDefaultPosition,
			wxDefaultSize,
			wxTAB_TRAVERSAL | wxNO_BORDER
		)
	);

	wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);

	/*
	 * Here's where we add widgets to top_sizer
	 */

	top_panel->SetSizer(top_sizer);
	top_sizer->Fit(top_panel);
	top_sizer->SetSizeHints(top_panel);

	// Show it
	frame->Show(true);

	// Start the event loop
	return true;
}



}  // namespace gui
}  // namespace phatbooks

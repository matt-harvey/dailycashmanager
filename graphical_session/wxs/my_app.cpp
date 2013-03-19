#include "my_app.hpp"
#include "account_list.hpp"
#include "entry_list.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <wx/snglinst.h>
#include <wx/wx.h>


using boost::shared_ptr;

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

	// Prevent multiple instances run by the same user
	wxString const app_name_plus_user_id =
		app_name + wxString::Format("-%s", wxGetUserId().c_str());
	m_checker = new wxSingleInstanceChecker(app_name_plus_user_id);
	if (m_checker->IsAnotherRunning())
	{
		wxLogError(app_name + " is already running.");
		return false;
	}

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
			wxFULL_REPAINT_ON_RESIZE
		)
	);

	wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);

	// Here's where we add widgets to frame
	
	AccountList* pl_account_list = create_pl_account_list
	(	top_panel,
		database_connection()
	);
	EntryList* act_ord_entry_list = create_actual_ordinary_entry_list
	(	top_panel,
		database_connection()
	);
	AccountList* bs_account_list = create_balance_sheet_account_list
	(	top_panel,
		database_connection()
	);

	// WARNING The sizer doesn't seem to be doing much here
	top_sizer->Add(pl_account_list, wxSizerFlags(1).Expand());
	top_sizer->Add(act_ord_entry_list, wxSizerFlags(2).Expand());
	top_sizer->Add(bs_account_list, wxSizerFlags(1).Expand());

	// Then we set this as panel's sizer
	top_panel->SetSizer(top_sizer);

	top_sizer->Fit(top_panel);
	top_sizer->SetSizeHints(top_panel);

	// Show it
	frame->Show(true);

	// Start the event loop
	return true;
}


void
MyApp::set_database_connection
(	shared_ptr<PhatbooksDatabaseConnection> p_database_connection
)
{
	m_database_connection = p_database_connection;
	return;
}

int MyApp::OnExit()
{
	delete m_checker;
	return 0;
}

}  // namespace gui
}  // namespace phatbooks

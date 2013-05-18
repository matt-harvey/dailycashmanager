#ifndef GUARD_frame_hpp
#define GUARD_frame_hpp

#include <wx/menu.h>
#include <wx/wx.h>
#include <wx/string.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class TopPanel;

// End forward declarations

class Frame:
	public wxFrame
{
public:

	Frame
	(	wxString const& title,
		PhatbooksDatabaseConnection& p_database_connection
	);

	// Event handlers
	void on_quit(wxCommandEvent& event);
	void on_about(wxCommandEvent& event);

private:

	PhatbooksDatabaseConnection& m_database_connection;
	TopPanel* m_top_panel;

	wxMenuBar* m_menu_bar;
	wxMenu* m_file_menu;
	wxMenu* m_help_menu;

};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp

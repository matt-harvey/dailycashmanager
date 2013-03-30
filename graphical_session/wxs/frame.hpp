#ifndef GUARD_frame_hpp
#define GUARD_frame_hpp

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
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

private:

	PhatbooksDatabaseConnection& m_database_connection;
	TopPanel* m_top_panel;

};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp

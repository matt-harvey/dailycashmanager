#ifndef GUARD_my_frame_hpp
#define GUARD_my_frame_hpp

#include <wx/wx.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

class MyFrame:
	public wxFrame
{
public:

	// Constructor
	MyFrame(wxString const& title);

	// Event handlers
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

private:
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_my_frame_hpp

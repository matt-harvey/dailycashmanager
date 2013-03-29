#ifndef GUARD_frame_hpp
#define GUARD_frame_hpp

#include <wx/wx.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

class Frame:
	public wxFrame
{
public:

	// Constructor
	Frame(wxString const& title);

	// Event handlers
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

private:
};


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frame_hpp

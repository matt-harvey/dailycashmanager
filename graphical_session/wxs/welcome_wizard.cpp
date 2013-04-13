#include "welcome_wizard.hpp"
#include "icon.xpm"
#include "frame.hpp"
#include <wx/wizard.h>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

WelcomeWizard::WelcomeWizard(Frame* parent):
	wxWizard
	(	parent,
		wxID_ANY,
		wxString("Welcome!"),
		wxBitmap(icon_xpm),  // TODO Put a proper image here
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	)
{
}


}  // namespace gui
}  // namesapce phatbooks



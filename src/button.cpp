#include "gui/button.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

// TODO LOW PRIORITY There is code shared between this and ComboBox,
// which might be factored out.

BEGIN_EVENT_TABLE(Button, wxButton)
	EVT_CHAR(Button::on_char)
END_EVENT_TABLE()

Button::Button
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxString const& p_label,
	wxPoint const& p_position,
	wxSize const& p_size,
	long p_style,
	wxValidator const& p_validator,
	wxString const& p_name
):
	wxButton
	(	p_parent,
		p_id,
		p_label,
		p_position,
		p_size,
		p_style,
		p_validator,
		p_name
	)
{
}

Button::~Button()
{
}

void
Button::on_char(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_TAB)
	{
		if (event.ShiftDown())
		{
			Navigate(wxNavigationKeyEvent::IsBackward);
		}
		else
		{
			Navigate(wxNavigationKeyEvent::IsForward);
		}
	}
	else
	{
		event.Skip();
	}
	return;
}

}  // namespace gui
}  // namespace dcm

#include "gui/combo_box.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(ComboBox, wxComboBox)
	EVT_CHAR(ComboBox::on_char)
END_EVENT_TABLE()

ComboBox::ComboBox
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxString const& p_value,
	wxPoint const& p_position,
	wxSize const& p_size,
	int p_n,
	wxString const p_choices[],
	long p_style,
	wxValidator const& p_validator,
	wxString const& p_name
):
	wxComboBox
	(	p_parent,
		p_id,
		p_value,
		p_position,
		p_size,
		p_n,
		p_choices,
		p_style | wxWANTS_CHARS,
		p_validator,
		p_name
	)
{
}

ComboBox::ComboBox
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxString const& p_value,
	wxPoint const& p_pos,
	wxSize const& p_size,
	wxArrayString const& p_choices,
	long p_style,
	wxValidator const& p_validator,
	wxString const& p_name
):
	wxComboBox
	(	p_parent,
		p_id,
		p_value,
		p_pos,
		p_size,
		p_choices,
		p_style | wxWANTS_CHARS,
		p_validator,
		p_name
	)
{
}

ComboBox::~ComboBox()
{
}

void
ComboBox::on_char(wxKeyEvent& event)
{
	JEWEL_LOG_TRACE();
	if (event.GetKeyCode() == WXK_TAB)
	{
		if (event.ShiftDown())
		{
			JEWEL_LOG_TRACE();
			// TODO
		}
		else
		{
			JEWEL_LOG_TRACE();
			// TODO
		}
	}
	else
	{
		JEWEL_LOG_TRACE();
		event.Skip();
	}
	JEWEL_LOG_TRACE();
	return;
}

}  // namespace gui
}  // namespace phatbooks

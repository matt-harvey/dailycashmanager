#include "gui/combo_box.hpp"
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

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
		p_style,
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
		p_style,
		p_validator,
		p_name
	)
{
}

ComboBox::~ComboBox() = default;

}  // namespace gui
}  // namespace phatbooks

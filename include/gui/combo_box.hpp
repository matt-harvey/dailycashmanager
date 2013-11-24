#ifndef GUARD_combo_box_hpp_04064646039188209
#define GUARD_combo_box_hpp_04064646039188209

#include <wx/combobox.h>
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

/**
 * The sole purpose of this class's existence is to enable us to intercept
 * the tab key manually, since on Windows (at least on some machines), the
 * tab key was not being properly processed for tab traversal by wxComboBox
 * itself.
 */
class ComboBox: public wxComboBox
{
public:
	ComboBox
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxString const& p_value = "",
		wxPoint const& p_position = wxDefaultPosition,
		wxSize const& p_size = wxDefaultSize,
		int p_n = 0,
		wxString const p_choices[] = nullptr,
		long p_style = 0,
		wxValidator const& p_validator = wxDefaultValidator,
		wxString const& p_name = "comboBox"
	);
	ComboBox
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxString const& p_value,
		wxPoint const& p_pos,
		wxSize const& p_size,
		wxArrayString const& p_choices,
		long p_style = 0,
		wxValidator const& p_validator = wxDefaultValidator,
		wxString const& p_name = "comboBox"
	);
	ComboBox(ComboBox const& rhs) = delete;
	ComboBox(ComboBox&& rhs) = delete;
	ComboBox& operator=(ComboBox const& rhs) = delete;
	ComboBox& operator=(ComboBox&& rhs) = delete;
	virtual ~ComboBox();

private:

	void on_char(wxKeyEvent& event);

	DECLARE_EVENT_TABLE();

};  // class ComboBox

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_combo_box_hpp_04064646039188209

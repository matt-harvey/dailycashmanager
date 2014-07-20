/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_text_ctrl_hpp_759744027983278
#define GUARD_text_ctrl_hpp_759744027983278

#include <wx/textctrl.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

class TextCtrl: public wxTextCtrl
{
public:

	TextCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxString const& p_value = "",
		wxPoint const& p_pos = wxDefaultPosition,
		wxSize const& p_size = wxDefaultSize,
		long p_style = 0,
		wxValidator const& p_validator = wxDefaultValidator,
		wxString const& p_name = wxTextCtrlNameStr
	);

	TextCtrl(TextCtrl const& rhs) = delete;
	TextCtrl(TextCtrl&& rhs) = delete;
	TextCtrl& operator=(TextCtrl const& rhs) = delete;
	TextCtrl& operator=(TextCtrl&& rhs) = delete;
	virtual ~TextCtrl();

private:

	void on_char(wxKeyEvent& event);
	void on_set_focus(wxFocusEvent& event);

	DECLARE_EVENT_TABLE();

};  // class TextCtrl

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_text_ctrl_hpp_759744027983278

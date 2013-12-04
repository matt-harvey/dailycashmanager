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

#ifndef GUARD_button_hpp_8649739566561645
#define GUARD_button_hpp_8649739566561645

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

/**
 * The sole purpose of this class's existence is to enable use to intercept
 * the tab key manually, since on Windows (at least on some machines), the
 * tab key was not being properly processed for tab traversal by
 * wxButton itself.
 */
class Button: public wxButton
{
public:
	Button
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxString const& p_label = wxEmptyString,
		wxPoint const& p_position = wxDefaultPosition,
		wxSize const& p_size = wxDefaultSize,
		long p_style = 0,
		wxValidator const& p_validator = wxDefaultValidator,
		wxString const& p_name = "button"
	);

	Button(Button const& rhs) = delete;
	Button(Button&& rhs) = delete;
	Button& operator=(Button const& rhs) = delete;
	Button& operator=(Button&& rhs) = delete;
	virtual ~Button();

private:
	
	void on_char(wxKeyEvent& event);

	DECLARE_EVENT_TABLE();

};  // class Button

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_button_hpp_8649739566561645

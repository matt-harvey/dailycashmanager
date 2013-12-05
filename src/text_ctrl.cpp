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

#include "gui/text_ctrl.hpp"
#include <jewel/log.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/validate.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(TextCtrl, wxTextCtrl)
	EVT_CHAR(TextCtrl::on_char)
END_EVENT_TABLE()

TextCtrl::TextCtrl
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxString const& p_value,
	wxPoint const& p_pos,
	wxSize const& p_size,
	long p_style,
	wxValidator const& p_validator,
	wxString const& p_name
):
	wxTextCtrl
	(	p_parent,
		p_id,
		p_value,
		p_pos,
		p_size,
		p_style | wxWANTS_CHARS,
		p_validator,
		p_name
	)
{
}

TextCtrl::~TextCtrl()
{
}

void
TextCtrl::on_char(wxKeyEvent& event)
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

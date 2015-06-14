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

// TODO LOW PRIORITY There is code shared between this, CheckBox and ComboBox,
// which might be factored out.

BEGIN_EVENT_TABLE(Button, wxButton)
    EVT_CHAR(Button::on_char)
END_EVENT_TABLE()

Button::Button
(   wxWindow* p_parent,
    wxWindowID p_id,
    wxString const& p_label,
    wxPoint const& p_position,
    wxSize const& p_size,
    long p_style,
    wxValidator const& p_validator,
    wxString const& p_name
):
    wxButton
    (   p_parent,
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
    auto const key_code = event.GetKeyCode();
    if (key_code == WXK_TAB)
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
    else if (key_code == WXK_RETURN)
    {
        // workaround for mysterious failure of Cancel, Save and Delete
        // buttons to respond to Enter in TransactionCtrl on Windows
        wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
        GetEventHandler()->ProcessEvent(e);
    }
    else
    {
        event.Skip();
    }
    return;
}

}  // namespace gui
}  // namespace dcm

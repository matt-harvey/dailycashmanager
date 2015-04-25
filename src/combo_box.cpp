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

#include "gui/combo_box.hpp"
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(ComboBox, wxComboBox)
    EVT_CHAR(ComboBox::on_char)
END_EVENT_TABLE()

ComboBox::ComboBox
(   wxWindow* p_parent,
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
    (   p_parent,
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
(   wxWindow* p_parent,
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
    (   p_parent,
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

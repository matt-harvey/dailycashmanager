/*
 * Copyright 2015 Matthew Harvey
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

#ifndef GUARD_check_box_hpp_8219739966560642
#define GUARD_check_box_hpp_8219739966560642

#include <wx/checkbox.h>
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
 * wxCheckBox itself.
 */
class CheckBox: public wxCheckBox
{
public:
    CheckBox
    (   wxWindow* p_parent,
        wxWindowID p_id,
        wxString const& p_label = wxEmptyString,
        wxPoint const& p_position = wxDefaultPosition,
        wxSize const& p_size = wxDefaultSize,
        long p_style = 0,
        wxValidator const& p_validator = wxDefaultValidator,
        wxString const& p_name = "check_box"
    );

    CheckBox(CheckBox const& rhs) = delete;
    CheckBox(CheckBox&& rhs) = delete;
    CheckBox& operator=(CheckBox const& rhs) = delete;
    CheckBox& operator=(CheckBox&& rhs) = delete;
    virtual ~CheckBox();

private:

    void on_char(wxKeyEvent& event);

    DECLARE_EVENT_TABLE();

};  // class CheckBox

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_check_box_hpp_8219739966560642

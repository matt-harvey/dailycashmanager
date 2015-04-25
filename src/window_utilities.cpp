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

#include "gui/window_utilities.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/optional.hpp>
#include <wx/string.h>
#include <wx/window.h>

using boost::optional;
using jewel::value;

namespace dcm
{
namespace gui
{

void
toggle_enabled
(   wxWindow* p_window,
    bool p_enable,
    optional<wxString> const& p_tooltip_if_disabled
)
{
    JEWEL_ASSERT (p_window);
    if (p_enable)
    {
        p_window->Enable();
        p_window->SetToolTip(0);
    }
    else
    {
        p_window->Disable();
        if (p_tooltip_if_disabled)
        {
            p_window->SetToolTip(value(p_tooltip_if_disabled));
        }
    }
    return;
}

}  // namespace gui
}  // namespace dcm

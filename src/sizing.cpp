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

#include "gui/sizing.hpp"
#include <wx/gdicmn.h>
#include <wx/settings.h>

namespace dcm
{
namespace gui
{

namespace
{
    double screen_width_double()
    {
        return wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    }
    double screen_height_double()
    {
        return wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    }
}

int
standard_gap()
{
    return screen_width_double() / 320;
}

int
standard_border()
{
    return standard_gap();
}

int
small_width()
{
    return screen_width_double() / 40;
}

int
medium_width()
{
    return screen_width_double() / 10;
}

int
large_width()
{
    return medium_width() * 2 + standard_gap();
}

int
extra_large_width()
{
    return medium_width() * 3 + standard_gap() * 2;
}

wxSize
screen_size()
{
    return wxSize(screen_width_double(), screen_height_double());
}

int scrollbar_width_allowance()
{
    // TODO LOW PRIORITY Use conditional compilation to adapt this to
    // different platforms.
    return 20;
}

}  // namespace gui
}  // namespace dcm

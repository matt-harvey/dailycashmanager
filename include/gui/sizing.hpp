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

#ifndef GUARD_sizing_hpp_3696029835239067
#define GUARD_sizing_hpp_3696029835239067

/**
 * @file
 *
 * Utilities for storing standard sizes used in widgets.
 */

#include <wx/gdicmn.h>

namespace dcm
{
namespace gui
{

/**
 * @returns standard gap to be placed between sub-widgets within their
 * parent widget.
 */
int standard_gap();

/**
 * @returns standard border to be placed around the outside of a set of
 * sub-widgets, within their parent widget.
 */
int standard_border();

/**
 * @returns a standardized "medium" width suitable, for example, for
 * a DecimalTextCtrl. This is guaranteed to be an even integer.
 */
int medium_width();

/**
 * @returns a standardized "large" width suitable, for example, for
 * a TextCtrl for entering an Entry comment of a few words.
 */
int large_width();

/**
 * @returns a standardized "extra large" width suitable, for example,
 * for a TextCtrl for entering a comment that's a bit longer than
 * what would fit if one that is \e large_width() wide.
 */
int extra_large_width();

wxSize screen_size();

/**
 * @returns a width that is supposed to be as wide as, or slightly wider than,
 * a vertical scrollbar.
 */
int scrollbar_width_allowance();

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_sizing_hpp_3696029835239067

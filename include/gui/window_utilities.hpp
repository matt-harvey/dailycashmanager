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

#ifndef GUARD_window_utilities_hpp_5290374944587413
#define GUARD_window_utilities_hpp_5290374944587413

#include <boost/optional.hpp>
#include <wx/string.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

/**
 * @param p_window window which we are toggling.
 *
 * @param p_enable Pass this \e true to enable to window, or \e false to
 * disable it. Note, enabling will disable any tooltip!
 *
 * @param p_tooltip_when_disabled \e If we pass this a wxString, \e then
 * there will be tooltip which will be set on the window if and only
 * \e p_enable was passed true.
 *
 * Precondition: \e p_window should not be null.
 */
void toggle_enabled
(	wxWindow* p_window,
	bool p_enable = true,
	boost::optional<wxString> const& p_tooltip_if_disabled =
		boost::optional<wxString>()
);


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_window_utilities_hpp_5290374944587413

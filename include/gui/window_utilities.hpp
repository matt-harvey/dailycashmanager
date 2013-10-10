/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

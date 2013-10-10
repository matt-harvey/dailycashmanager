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


#include "gui/window_utilities.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/optional.hpp>
#include <wx/string.h>
#include <wx/window.h>

using boost::optional;
using jewel::value;

namespace phatbooks
{
namespace gui
{

void
toggle_enabled
(	wxWindow* p_window,
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
}  // namespace phatbooks

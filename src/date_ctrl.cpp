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


#include "gui/date_ctrl.hpp"
#include "phatbooks_exceptions.hpp"
#include "gui/date_validator.hpp"
#include "gui/locale.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <wx/datetime.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

using boost::optional;
namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(DateCtrl, wxTextCtrl)
	EVT_KILL_FOCUS(DateCtrl::on_kill_focus)
END_EVENT_TABLE()

DateCtrl::DateCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	wxSize const& p_size,
	gregorian::date const& p_date,
	bool p_allow_blank,
	optional<gregorian::date> const& p_min_date
):
	wxTextCtrl
	(	p_parent,
		p_id,
		date_format_wx(p_date),
		wxDefaultPosition,
		p_size,
		wxALIGN_RIGHT,
		DateValidator(p_date, p_allow_blank, p_min_date)
	)
{
}
	
optional<gregorian::date>
DateCtrl::date()
{
	DateValidator const* const validator =
		dynamic_cast<DateValidator const*>(GetValidator());
	JEWEL_ASSERT (validator);
	return validator->date();
}

void
DateCtrl::on_kill_focus(wxFocusEvent& event)
{
	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the DateCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
}

}  // namespace gui
}  // namespace phatbooks

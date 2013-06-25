// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_date_ctrl_hpp
#define GUARD_date_ctrl_hpp

#include "date.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

/**
 * Widget with which the user may select a date.
 *
 * @todo On Windows, at least, this is acting strangely. It
 * is flipping the day and the month around, at least in some
 * situations such as as used in TransactionCtrl.
 */
class DateCtrl: public wxTextCtrl
{
public:
	DateCtrl
	(	wxWindow* p_parent,
		unsigned int p_id,
		wxSize const& p_size,
		boost::gregorian::date const& p_date = today()
	);

	/**
	 * @returns a date from the control.
	 *
	 * Unfortunately this can't currently be const because the
	 * wxWindow::GetValidator() function used in the implementation,
	 * is not const.
	 */
	boost::gregorian::date date();

private:
	void on_kill_focus(wxFocusEvent& event);
	DECLARE_EVENT_TABLE()

};  // class DateCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_date_ctrl_hpp

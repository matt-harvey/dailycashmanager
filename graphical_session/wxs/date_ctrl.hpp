// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_date_ctrl_hpp
#define GUARD_date_ctrl_hpp

#include "date.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
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
 * @param p_allow_blank should be set to true if and only if we will
 * permit the user to leave the DateCtrl blank.
 */
class DateCtrl: public wxTextCtrl
{
public:
	DateCtrl
	(	wxWindow* p_parent,
		unsigned int p_id,
		wxSize const& p_size,
		boost::gregorian::date const& p_date = today(),
		bool p_allow_blank = false
	);

	/**
	 * @returns a boost::optional initialized with the date in the
	 * control, or uninitialized if the control does not show
	 * a date.
	 *
	 * Unfortunately this can't currently be const because the
	 * wxWindow::GetValidator() function used in the implementation,
	 * is not const.
	 */
	boost::optional<boost::gregorian::date> date();

private:
	void on_kill_focus(wxFocusEvent& event);
	DECLARE_EVENT_TABLE()

};  // class DateCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_date_ctrl_hpp

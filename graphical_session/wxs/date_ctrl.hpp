// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_date_ctrl_hpp_9775690613210057
#define GUARD_date_ctrl_hpp_9775690613210057

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

	/**
	 * @param p_parent parent window
	 *
	 * @param p_id window id
	 *
	 * @param p_size size
	 *
	 * @param p_date the date which will be initially displayed in the
	 * DateCtrl
	 *
	 * @param p_allow_blank Pass this \e true if and only if the user will
	 * be permitted to leave the DateCtrl blank
	 *
	 * @param p_min_date If this parameter is passed a date (or an initialized
	 * boost::optional<boost::gregorian::date>), then the user will not be
	 * permitted to enter a date earlier than that date. An error message
	 * will be displayed to the user if an attempt is made to do so.
	 */
	DateCtrl
	(	wxWindow* p_parent,
		unsigned int p_id,
		wxSize const& p_size,
		boost::gregorian::date const& p_date = today(),
		bool p_allow_blank = false,
		boost::optional<boost::gregorian::date> const& p_min_date =
			boost::optional<boost::gregorian::date>()
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

#endif  // GUARD_date_ctrl_hpp_9775690613210057

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

#ifndef GUARD_date_ctrl_hpp_9775690613210057
#define GUARD_date_ctrl_hpp_9775690613210057

#include "date.hpp"
#include "gui/text_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

/**
 * Widget with which the user may select a date.
 */
class DateCtrl: public TextCtrl
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
    (   wxWindow* p_parent,
        unsigned int p_id,
        wxSize const& p_size,
        boost::gregorian::date const& p_date = today(),
        bool p_allow_blank = false,
        boost::optional<boost::gregorian::date> const& p_min_date =
            boost::optional<boost::gregorian::date>()
    );

    DateCtrl(DateCtrl const&) = delete;
    DateCtrl(DateCtrl&&) = delete;
    DateCtrl& operator=(DateCtrl const&) = delete;
    DateCtrl& operator=(DateCtrl&&) = delete;
    ~DateCtrl() = default;

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
    void on_set_focus(wxFocusEvent& event);
    DECLARE_EVENT_TABLE()

};  // class DateCtrl

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_date_ctrl_hpp_9775690613210057

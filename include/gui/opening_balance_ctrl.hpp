/*
 * Copyright 2015 Matthew Harvey
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

#ifndef GUARD_opening_balance_ctrl_hpp_5700779848592719
#define GUARD_opening_balance_ctrl_hpp_5700779848592719

#include "gui/decimal_text_ctrl.hpp"
#include <jewel/decimal.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>

namespace dcm
{
namespace gui
{

class OpeningBalanceCtrl: public DecimalTextCtrl
{
public:
    OpeningBalanceCtrl
    (   wxWindow* p_parent,
        unsigned int p_id,
        wxSize const& p_size,
        jewel::Decimal::places_type p_precision,
        bool p_print_dash_for_zero = true
    );
	OpeningBalanceCtrl(OpeningBalanceCtrl const& rhs) = delete;
	OpeningBalanceCtrl(OpeningBalanceCtrl&& rhs) = delete;
	OpeningBalanceCtrl& operator=(OpeningBalanceCtrl const& rhs) = delete;
	OpeningBalanceCtrl& operator=(OpeningBalanceCtrl&& rhs) = delete;
	virtual ~OpeningBalanceCtrl();

private:
    virtual void on_kill_focus(wxFocusEvent& event) override;

    DECLARE_EVENT_TABLE()

};  // class OpeningBalanceCtrl

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_opening_balance_ctrl_hpp_5700779848592719

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

#include "gui/opening_balance_ctrl.hpp"
#include "gui/account_dialog.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/multi_account_panel.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/window.h>

using jewel::Decimal;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(OpeningBalanceCtrl, DecimalTextCtrl)
    EVT_KILL_FOCUS(OpeningBalanceCtrl::on_kill_focus)
END_EVENT_TABLE()

OpeningBalanceCtrl::OpeningBalanceCtrl
(   wxWindow* p_parent,
    unsigned int p_id,
    wxSize const& p_size,
    Decimal::places_type p_precision,
    bool p_print_dash_for_zero  // TODO LOW PRIORITY Would be cleaner with a FlagSet here.
):
    DecimalTextCtrl
    (   p_parent,
        p_id,
        p_size,
        p_precision,
        p_print_dash_for_zero
    )
{
}
    
OpeningBalanceCtrl::~OpeningBalanceCtrl()
{
}

void
OpeningBalanceCtrl::on_kill_focus(wxFocusEvent& event)
{
    // TODO LOW PRIORITY Parent widget relies on the call to
    // GetParent()->TransferDataToWindow() here. This coupling is ugly
    // and fragile. Improve this.
    event.Skip();
    auto const orig = amount();
    auto* const validator = GetValidator();
    auto* const parent = GetParent();
    JEWEL_ASSERT (parent);
    JEWEL_ASSERT (validator);
    bool ok = validator->Validate(static_cast<wxWindow*>(this));
    if (ok)
    {
        // TODO LOW PRIORITY. This dynamic casting stuff is a bit
        // messy. A better design would be for MultiAccountPanel to have
        // a custom widget per new account row, and have that handle wxFocusEvent.
        // Similarly, AccountDialog should itself handle the wxFocusEvent.
        // Both the the custom widget class, and AccountDialog, should have
        // a common base class in which the logic for warning the
        // user etc. is handled.
        auto const panel = dynamic_cast<gui::MultiAccountPanel*>(parent);
        auto const dialog = dynamic_cast<gui::AccountDialog*>(parent);
        if (panel || dialog)
        {
            ok = parent->TransferDataToWindow();
            auto const amt = amount();
            if (ok && (amt != orig))
            {
                JEWEL_ASSERT ((panel != nullptr) || (dialog != nullptr));
                auto const account_type =
                (   panel ?
                    panel->account_type_for(this) :
                    dialog->selected_account_type()
                );
                wxString message;
                if ((account_type == AccountType::asset) && (amt < Decimal(0, 0)))
                {
                    message = "Asset accounts normally have a positive balance. "
                        "Do you want to change this to a positive number?";
                }
                else if ((account_type == AccountType::liability) && (amt > Decimal(0, 0)))
                {
                    message = "Liability accounts normally have a negative balance. "
                        "Do you want to change this to a negative number?";
                }
                if (!message.empty())
                {
                    wxMessageDialog warning(panel, message, wxEmptyString, wxYES_NO);
                    auto const result = warning.ShowModal();
                    JEWEL_ASSERT ((result == wxID_YES) || (result == wxID_NO));
                    if (result == wxID_YES)
                    {
                        // TODO MEDIUM PRIORITY There is a tiny possibility that
                        // this will throw DecimalUnaryMinusException. If this
                        // happens, the application will simply crash. This
                        // is extremely unlikely ever to occur, but still,
                        // crashing is not really an acceptable reaction to this.
                        set_amount(-amt);
                    }
                }
            }
        }
        else
        {
            ok = validator->TransferToWindow();
        }
    }
    if (!ok) set_amount(orig);
    return;
}

}  // namespace gui
}  // namespace dcm

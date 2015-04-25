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

#include "gui/decimal_text_ctrl.hpp"
#include "finformat.hpp"
#include "dcm_exceptions.hpp"
#include "gui/budget_panel.hpp"
#include "gui/decimal_validator.hpp"
#include "gui/locale.hpp"
#include "gui/multi_account_panel.hpp"
#include "gui/text_ctrl.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <iostream>

using jewel::Decimal;
using std::endl;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(DecimalTextCtrl, TextCtrl)
    EVT_KILL_FOCUS(DecimalTextCtrl::on_kill_focus)
    EVT_SET_FOCUS(DecimalTextCtrl::on_set_focus)
END_EVENT_TABLE()

DecimalTextCtrl::DecimalTextCtrl
(   wxWindow* p_parent,
    unsigned int p_id,
    wxSize const& p_size,
    Decimal::places_type p_precision,
    bool p_print_dash_for_zero  // TODO LOW PRIORITY Would be cleaner with a FlagSet here.
):
    TextCtrl
    (   p_parent,
        p_id,
        finformat_wx
        (   Decimal(0, p_precision),
            locale(),
            (   p_print_dash_for_zero?
                DecimalFormatFlags():
                DecimalFormatFlags().clear(string_flags::dash_for_zero)
            )
        ),
        wxDefaultPosition,
        p_size,
        wxALIGN_RIGHT,
        DecimalValidator
        (   Decimal(0, p_precision),
            p_precision,
            p_print_dash_for_zero
        )
    ),
    m_print_dash_for_zero(p_print_dash_for_zero),
    m_precision(p_precision)
{
}

DecimalTextCtrl::~DecimalTextCtrl()
{
}

void
DecimalTextCtrl::set_amount(Decimal const& p_amount)
{
    Decimal::places_type const prec = p_amount.places();
    if (prec != m_precision)
    {
        DecimalValidator* const validator =
            dynamic_cast<DecimalValidator*>(GetValidator());    
        JEWEL_ASSERT (validator);
        m_precision = prec;
        validator->set_precision(prec);
    }
    JEWEL_ASSERT (p_amount.places() == m_precision);
    DecimalFormatFlags flags;
    if (!m_print_dash_for_zero) flags.clear(string_flags::dash_for_zero);
    SetValue(finformat_wx(p_amount, locale(), flags));

    // TODO LOW PRIORITY This really sucks. We are validating the entire
    // parent window as a side-effect of setting the value of just one
    // of its children. But if we call Validate() on DecimalTextCtrl directly
    // it doesn't have any effect (for some reason).
    GetParent()->Validate();

    return;
}

Decimal
DecimalTextCtrl::amount()
{
    DecimalValidator const* const validator =
        dynamic_cast<DecimalValidator const*>(GetValidator());
    JEWEL_ASSERT (validator);
    return validator->decimal();
}

void
DecimalTextCtrl::on_kill_focus(wxFocusEvent& event)
{
    // TODO LOW PRIORITY BudgetPanel and MultiAccountPanel rely on the call to
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
        if
        (   dynamic_cast<gui::BudgetPanel*>(parent) ||
            dynamic_cast<gui::MultiAccountPanel*>(parent)
        )
        {
            ok = parent->TransferDataToWindow();
        }
        else
        {
            ok = validator->TransferToWindow();
        }
    }
    if (!ok) set_amount(orig);
    return;
}

void
DecimalTextCtrl::on_set_focus(wxFocusEvent& event)
{
    event.Skip();
    SelectAll();
    return;
}

}  // namespace gui
}  // namespace dcm

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

#include "gui/decimal_validator.hpp"
#include "app.hpp"
#include "finformat.hpp"
#include "gui/locale.hpp"
#include "gui/text_ctrl.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/exception.hpp>
#include <wx/msgdlg.h>
#include <wx/valtext.h>

using jewel::Decimal;
using jewel::round;

namespace dcm
{
namespace gui
{

DecimalValidator::DecimalValidator
(   Decimal const& p_decimal,
    Decimal::places_type p_precision,
    bool p_print_dash_for_zero
):
    m_print_dash_for_zero(p_print_dash_for_zero),
    m_precision(p_precision),
    m_decimal(p_decimal)
{
}

Decimal
DecimalValidator::decimal() const
{
    return m_decimal;
}

void
DecimalValidator::set_precision(jewel::Decimal::places_type p_precision)
{
    m_precision = p_precision;
    m_decimal = round(m_decimal, p_precision);
    return;
}

bool
DecimalValidator::Validate(wxWindow* WXUNUSED(parent))
{
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    TextCtrl const* const text_ctrl = dynamic_cast<TextCtrl*>(GetWindow());
    if (!text_ctrl)
    {
        return false;
    }
    try
    {
        m_decimal = round
        (   wx_to_decimal(wxString(text_ctrl->GetValue()), locale()),
            m_precision
        );
        return true;
    }
    catch (jewel::Exception&)
    {
        wxString const text(text_ctrl->GetValue());
        try
        {
            m_decimal = round(wx_to_simple_sum(text, locale()), m_precision);
            return true;
        }
        catch (jewel::DecimalFromStringException&)
        {
            wxMessageBox
            (   wxString("Could not interpret \"") + text + "\" as a number."
            );
            return false;
        }
        catch (jewel::DecimalException&)
        {
            wxMessageBox
            (   wxString("Could not safely read \"") +
                text +
                "\" as a number. Number may be too large or contain " +
                "unexpected characters."
            );
            return false;
        }
        catch (jewel::Exception&)
        {
            wxMessageBox("There was an error reading the number you entered.");
            return false;
        }
    }
    JEWEL_HARD_ASSERT (false);
}

bool
DecimalValidator::TransferFromWindow()
{
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    return true;
}

bool
DecimalValidator::TransferToWindow()
{
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    TextCtrl* const text_ctrl = dynamic_cast<TextCtrl*>(GetWindow());
    if (!text_ctrl)
    {
        return false;
    }
    DecimalFormatFlags flags;
    if (!m_print_dash_for_zero) flags.clear(string_flags::dash_for_zero);
    text_ctrl->SetValue(finformat_wx(m_decimal, locale(), flags));
    return true;
}

wxObject*
DecimalValidator::Clone() const
{
    return new DecimalValidator(*this);
}



}  // namespace gui
}  // namespace dcm

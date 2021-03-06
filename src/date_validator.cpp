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

#include "gui/date_validator.hpp"
#include "date.hpp"
#include "date_parser.hpp"
#include "gui/locale.hpp"
#include "gui/text_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/validate.h>
#include <iostream>

using boost::optional;
using jewel::clear;
using jewel::value;
using std::endl;

namespace gregorian = boost::gregorian;

namespace dcm
{
namespace gui
{

DateValidator::DateValidator
(   gregorian::date const& p_date,
    bool p_allow_blank,
    optional<gregorian::date> const& p_min_date
):
    m_allow_blank(p_allow_blank),
    m_date(p_date),
    m_min_date(p_min_date)
{
}

DateValidator::DateValidator(DateValidator const& rhs):
    wxValidator(),
    m_allow_blank(rhs.m_allow_blank),
    m_date(rhs.m_date),
    m_min_date(rhs.m_min_date)
{
}

bool
DateValidator::Validate(wxWindow* WXUNUSED(parent))
{
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    TextCtrl const* const text_ctrl =
        dynamic_cast<TextCtrl*>(GetWindow());
    if (!text_ctrl)
    {
        return false;
    }
    wxString const date_text(text_ctrl->GetValue());
    if (m_allow_blank && date_text.IsEmpty())
    {
        clear(m_date);        
        return true;
    }
    DateParser const parser;
    optional<gregorian::date> temp = parser.parse(date_text, true);
    if (!temp)
    {
        wxMessageBox
        (   wxString("\"") +
            date_text +
            wxString("\" ") +
            wxString("is not recognised as a date.")
        );
        return false;
    }
    JEWEL_ASSERT (temp);
    if (m_min_date && (value(temp) < value(m_min_date)))
    {
        // TODO HIGH PRIORITY This message doesn't actually explain to the
        // user why they can't enter a date that is this early.
        wxMessageBox
        (   wxString("Date should not be earlier than ") +
            date_format_wx(value(m_min_date)) +
            wxString(".")
        );
        return false;
    }
    m_date = temp;
    return true;
}

bool
DateValidator::TransferFromWindow()
{
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    return true;
}

bool
DateValidator::TransferToWindow()
{
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    TextCtrl* const text_ctrl = dynamic_cast<TextCtrl*>(GetWindow());
    if (!text_ctrl)
    {
        return false;
    }
    if (!m_date)
    {
        if (!m_allow_blank)
        {
            return false;
        }
        JEWEL_ASSERT (m_allow_blank && !m_date);
        text_ctrl->SetValue(wxEmptyString);
        return true;
    }
    JEWEL_ASSERT (m_date);
    JEWEL_ASSERT (!m_min_date || (m_date >= value(m_min_date)));
    text_ctrl->SetValue(date_format_wx(value(m_date)));
    return true;
}

wxObject*
DateValidator::Clone() const
{
    return new DateValidator(*this);
}

optional<gregorian::date>
DateValidator::date() const
{
    return m_date;
}

}  // namespace gui
}  // namespace dcm

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

#ifndef GUARD_summary_datum_hpp_35246402786693726
#define GUARD_summary_datum_hpp_35246402786693726

#include <jewel/decimal.hpp>
#include <wx/string.h>

namespace dcm
{
namespace gui
{

/**
 * Represents a single piece of GUI-displayable financial information,
 * along with a label for that information.
 */
class SummaryDatum
{
public:
    SummaryDatum(wxString const& p_label, jewel::Decimal const& p_amount);
    // synthesised copy constructor, copy assignment and destructor are OK
    // ignore move constructor
    // ignore move assignment
    wxString label() const;
    jewel::Decimal amount() const;
    void set_amount(jewel::Decimal const& p_amount);

private:

    // declare m_label before m_amount to ensure synthesised assignment
    // operator is exception-safe
    wxString m_label;

    jewel::Decimal m_amount;

};  // class SummaryDatum


}  // namespace gui
}  // namespace dcm

#endif  // GUARD_summary_datum_hpp_35246402786693726

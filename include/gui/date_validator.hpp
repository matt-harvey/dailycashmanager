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

#ifndef GUARD_date_validator_hpp_1494637937981001
#define GUARD_date_validator_hpp_1494637937981001

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/validate.h>

namespace dcm
{
namespace gui
{

/**
 * Validates a wxString purporting to represent a date.
 */
class DateValidator: public wxValidator
{
public:
    DateValidator
    (   boost::gregorian::date const& p_date,
        bool p_allow_blank,
        boost::optional<boost::gregorian::date> const& p_min_date =
            boost::optional<boost::gregorian::date>()
    );

    DateValidator(DateValidator const& rhs);

    DateValidator(DateValidator&&) = delete;
    DateValidator& operator=(DateValidator const&) = delete;
    DateValidator& operator=(DateValidator&&) = delete;
    ~DateValidator() = default;

    /**
     * @param parent should point to an instance of a TextCtrl.
     */
    bool Validate(wxWindow* parent) override;

    bool TransferFromWindow() override;
    bool TransferToWindow() override;
    wxObject* Clone() const override;

    boost::optional<boost::gregorian::date> date() const;

private:
    bool m_allow_blank;
    boost::optional<boost::gregorian::date> m_date;
    boost::optional<boost::gregorian::date> m_min_date;

};  // class DateValidator

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_date_validator_hpp_1494637937981001

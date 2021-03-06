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

#ifndef GUARD_string_set_validator_hpp_5293149675844773
#define GUARD_string_set_validator_hpp_5293149675844773

#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/validate.h>

namespace dcm
{
namespace gui
{

/**
 * Validates a wxString by seeing if it is identical, case insensitively,
 * to some element of a given wxArrayString. Provides autocompletion,
 * and displays an error message to the user if the text under consideration
 * is invalid.
 */
class StringSetValidator: public wxValidator
{
public:
    /**
     * @param p_text initial text contents of validator
     *
     * @param p_valid_strings strings considered valid
     *
     * @param p_description_for_error_message a noun phrase to be incorporated
     * into the error message displayed to the user when text is invalid. E.g.
     * if p_valid_strings is a list of Account names, then
     * p_description_for_error_message might be something like "account".
     */
    StringSetValidator
    (   wxString const& p_text,
        wxArrayString const& p_valid_strings,
        wxString const& p_description_for_error_message
    );

    StringSetValidator(StringSetValidator const& rhs);

    StringSetValidator(StringSetValidator&&) = delete;
    StringSetValidator& operator=(StringSetValidator const&) = delete;
    StringSetValidator& operator=(StringSetValidator&&) = delete;

    ~StringSetValidator() = default;

    /**
     * @param parent should point to an instance of wxTextEntry.
     */
    bool Validate(wxWindow* parent) override;

    bool TransferFromWindow() override;
    bool TransferToWindow() override;
    wxObject* Clone() const override;

private:
    wxString m_text;
    wxArrayString m_valid_strings;
    wxString m_description_for_error_message;

};  // class StringSetValidator

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_string_set_validator_hpp_5293149675844773

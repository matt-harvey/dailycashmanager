/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_string_set_validator_hpp_5293149675844773
#define GUARD_string_set_validator_hpp_5293149675844773

#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/validate.h>

namespace phatbooks
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
	(	wxString const& p_text,
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
}  // namespace phatbooks

#endif  // GUARD_string_set_validator_hpp_5293149675844773

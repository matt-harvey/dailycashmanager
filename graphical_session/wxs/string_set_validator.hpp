#ifndef GUARD_string_set_validator_hpp
#define GUARD_string_set_validator_hpp

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

	/**
	 * @param parent should point to an instance of wxTextEntry.
	 */
	bool Validate(wxWindow* parent);

	bool TransferFromWindow();
	bool TransferToWindow();
	wxObject* Clone() const;

	// TODO This isn't working. Need to fix.
	// wxString text() const;

private:
	wxString m_text;
	wxArrayString m_valid_strings;
	wxString m_description_for_error_message;

};  // class StringSetValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_string_set_validator_hpp

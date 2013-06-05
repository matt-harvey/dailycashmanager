#ifndef GUARD_account_name_validator_hpp
#define GUARD_account_name_validator_hpp

#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/validate.h>

namespace phatbooks
{
namespace gui
{

/**
 * Validates a wxString purporting to be the name of an
 * Account, within a given set of permissible Account names.
 *
 * @todo There is nothing in particular about this class which means
 * we must use it for Account names. It could be used for validating
 * against any sequence of wxStrings. We should probably embrace this
 * generality, and rename the class accordingly.
 */
class AccountNameValidator: public wxValidator
{
public:
	AccountNameValidator
	(	wxString const& p_account_name,
		wxArrayString const& p_valid_account_names
	);
	AccountNameValidator(AccountNameValidator const& rhs);

	/**
	 * @param parent should point to an instance of wxTextEntry.
	 */
	bool Validate(wxWindow* parent);

	bool TransferFromWindow();
	bool TransferToWindow();
	wxObject* Clone() const;

	wxString account_name() const;

private:
	wxString m_account_name;
	wxArrayString m_valid_account_names;

};  // class AccountNameValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_name_validator_hpp

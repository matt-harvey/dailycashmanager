#include "account_name_validator.hpp"
#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/validate.h>

namespace phatbooks
{
namespace gui
{


AccountNameValidator::AccountNameValidator
(	wxString const& p_account_name,
	wxArrayString const& p_valid_account_names
):
	m_account_name(p_account_name),
	m_valid_account_names(p_valid_account_names)
{
}

AccountNameValidator::AccountNameValidator(AccountNameValidator const& rhs):
	wxValidator(),
	m_account_name(rhs.m_account_name),
	m_valid_account_names(rhs.m_valid_account_names)
{
}

bool
AccountNameValidator::Validate(wxWindow* WXUNUSED(wxparent))
{
	wxTextEntry const* const text_entry =
		dynamic_cast<wxTextEntry*>(GetWindow());
	assert (text_entry);
	wxString const text(text_entry->GetValue());
	int position = wxNOT_FOUND;
	if ((position = m_valid_account_names.Index(text)) == wxNOT_FOUND)
	{
		wxMessageBox
		(	wxString("\"") +
			text +
			wxString("\" ") +
			wxString("is not a valid account or category name.")
		);
		return false;
	}
	assert (position != wxNOT_FOUND);
	m_account_name = text;
	return true;
}

bool
AccountNameValidator::TransferFromWindow()
{
	assert (dynamic_cast<wxTextEntry*>(GetWindow()));
	return true;
}

bool
AccountNameValidator::TransferToWindow()
{
	wxTextEntry* const text_entry = dynamic_cast<wxTextEntry*>(GetWindow());
	assert (text_entry);
	text_entry->SetValue(m_account_name);
	return true;
}

wxObject*
AccountNameValidator::Clone() const
{
	return new AccountNameValidator(*this);
}

wxString
AccountNameValidator::account_name() const
{
	return m_account_name;
}



}  // namespace gui
}  // namespace phatbooks

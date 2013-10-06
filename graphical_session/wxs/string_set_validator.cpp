// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "string_set_validator.hpp"
#include <jewel/assert.hpp>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/validate.h>

namespace phatbooks
{
namespace gui
{


StringSetValidator::StringSetValidator
(	wxString const& p_text,
	wxArrayString const& p_valid_strings,
	wxString const& p_description_for_error_message
):
	m_text(p_text),
	m_valid_strings(p_valid_strings),
	m_description_for_error_message(p_description_for_error_message)
{
}

StringSetValidator::StringSetValidator(StringSetValidator const& rhs):
	wxValidator(),
	m_text(rhs.m_text),
	m_valid_strings(rhs.m_valid_strings),
	m_description_for_error_message(rhs.m_description_for_error_message)
{
}

bool
StringSetValidator::Validate(wxWindow* WXUNUSED(wxparent))
{
	wxTextEntry const* const text_entry =
		dynamic_cast<wxTextEntry*>(GetWindow());
	JEWEL_ASSERT (text_entry);
	wxString const candidate_text(text_entry->GetValue());
	int position = wxNOT_FOUND;
	if ((position = m_valid_strings.Index(candidate_text)) == wxNOT_FOUND)
	{
		wxMessageBox
		(	wxString("\"") +
			candidate_text +
			wxString("\" ") +
			wxString("is not a valid ") + m_description_for_error_message
		);
		return false;
	}
	JEWEL_ASSERT (position != wxNOT_FOUND);
	m_text = candidate_text;
	return true;
}

bool
StringSetValidator::TransferFromWindow()
{
	wxTextEntry* const text_entry = dynamic_cast<wxTextEntry*>(GetWindow());
	JEWEL_ASSERT (text_entry);	
	m_text = text_entry->GetValue();
	return true;
}

bool
StringSetValidator::TransferToWindow()
{
	wxTextEntry* const text_entry = dynamic_cast<wxTextEntry*>(GetWindow());
	JEWEL_ASSERT (text_entry);
	text_entry->SetValue(m_text);
	return true;
}

wxObject*
StringSetValidator::Clone() const
{
	return new StringSetValidator(*this);
}


}  // namespace gui
}  // namespace phatbooks

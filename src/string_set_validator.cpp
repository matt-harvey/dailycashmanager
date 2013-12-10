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

#include "gui/string_set_validator.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/validate.h>

namespace dcm
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
	JEWEL_LOG_TRACE();
	wxTextEntry const* const text_entry =
		dynamic_cast<wxTextEntry*>(GetWindow());
	JEWEL_ASSERT (text_entry);
	wxString const candidate_text(text_entry->GetValue());
	int pos = wxNOT_FOUND;
	if ((pos = m_valid_strings.Index(candidate_text, false)) == wxNOT_FOUND)
	{
		wxMessageBox
		(	wxString("\"") +
			candidate_text +
			wxString("\" ") +
			wxString("is not a valid ") + m_description_for_error_message
		);
		JEWEL_LOG_TRACE();
		return false;
	}
	JEWEL_ASSERT (pos != wxNOT_FOUND);
	JEWEL_ASSERT (pos >= 0);
	JEWEL_ASSERT (static_cast<unsigned int>(pos) < m_valid_strings.GetCount());
	m_text = m_valid_strings.Item(pos);
	JEWEL_LOG_TRACE();
	return true;
}

bool
StringSetValidator::TransferFromWindow()
{
	JEWEL_LOG_TRACE();
	wxTextEntry* const text_entry = dynamic_cast<wxTextEntry*>(GetWindow());
	JEWEL_ASSERT (text_entry);	
	m_text = text_entry->GetValue();
	return true;
}

bool
StringSetValidator::TransferToWindow()
{
	JEWEL_LOG_TRACE();
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
}  // namespace dcm

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


#include "gui/string_set_validator.hpp"
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
	JEWEL_LOG_TRACE();
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
		JEWEL_LOG_TRACE();
		return false;
	}
	JEWEL_ASSERT (position != wxNOT_FOUND);
	m_text = candidate_text;
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
}  // namespace phatbooks

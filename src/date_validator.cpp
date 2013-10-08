// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "gui/date_validator.hpp"
#include "date.hpp"
#include "date_parser.hpp"
#include "gui/locale.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include <wx/validate.h>
#include <iostream>

using boost::optional;
using jewel::clear;
using jewel::value;
using std::endl;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

DateValidator::DateValidator
(	gregorian::date const& p_date,
	bool p_allow_blank,
	optional<gregorian::date> const& p_min_date
):
	m_allow_blank(p_allow_blank),
	m_date(p_date),
	m_min_date(p_min_date)
{
}

DateValidator::DateValidator(DateValidator const& rhs):
	wxValidator(),
	m_allow_blank(rhs.m_allow_blank),
	m_date(rhs.m_date),
	m_min_date(rhs.m_min_date)
{
}

bool
DateValidator::Validate(wxWindow* WXUNUSED(parent))
{
	// TODO Make this more accepting of different formats.
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl const* const text_ctrl =
		dynamic_cast<wxTextCtrl*>(GetWindow());
	if (!text_ctrl)
	{
		return false;
	}
	wxString const date_text(text_ctrl->GetValue());
	if (m_allow_blank && date_text.IsEmpty())
	{
		clear(m_date);		
		return true;
	}
	DateParser const parser;
	optional<gregorian::date> temp = parser.parse(date_text, true);
	if (!temp)
	{
		wxMessageBox
		(	wxString("\"") +
			date_text +
			wxString("\" ") +
			wxString("is not recognised as a date.")
		);
		return false;
	}
	JEWEL_ASSERT (temp);
	if (m_min_date && (value(temp) < value(m_min_date)))
	{
		// TODO This message doesn't actually explain to the user
		// why they can't enter a date that is this early.
		wxMessageBox
		(	wxString("Date should not be earlier than ") +
			date_format_wx(value(m_min_date)) +
			wxString(".")
		);
		return false;
	}
	m_date = temp;
	return true;
}

bool
DateValidator::TransferFromWindow()
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	return true;
}

bool
DateValidator::TransferToWindow()
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl* const text_ctrl = dynamic_cast<wxTextCtrl*>(GetWindow());
	if (!text_ctrl)
	{
		return false;
	}
	if (!m_date)
	{
		if (!m_allow_blank)
		{
			return false;
		}
		JEWEL_ASSERT (m_allow_blank && !m_date);
		text_ctrl->SetValue(wxEmptyString);
		return true;
	}
	JEWEL_ASSERT (m_date);
	JEWEL_ASSERT (!m_min_date || (m_date >= value(m_min_date)));
	text_ctrl->SetValue(date_format_wx(value(m_date)));
	return true;
}

wxObject*
DateValidator::Clone() const
{
	return new DateValidator(*this);
}

optional<gregorian::date>
DateValidator::date() const
{
	return m_date;
}

}  // namespace gui
}  // namespace phatbooks

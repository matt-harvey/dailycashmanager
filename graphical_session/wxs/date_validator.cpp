// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "date_validator.hpp"
#include "date.hpp"
#include "locale.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include <wx/validate.h>
#include <cassert>
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
	bool p_allow_blank
):
	m_allow_blank(p_allow_blank),
	m_date(p_date)
{
}

DateValidator::DateValidator(DateValidator const& rhs):
	wxValidator(),
	m_allow_blank(rhs.m_allow_blank),
	m_date(rhs.m_date)
{
}

bool
DateValidator::Validate(wxWindow* WXUNUSED(parent))
{
	// TODO Make this more accepting of different formats.
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl const* const text_ctrl =
		dynamic_cast<wxTextCtrl*>(GetWindow());
	if (!text_ctrl)
	{
		return false;
	}
	wxString::const_iterator parsed_to_position;
	wxDateTime date_wx;
	wxString const date_text(text_ctrl->GetValue());
	if (m_allow_blank && date_text.IsEmpty())
	{
		clear(m_date);		
		return true;
	}
	wxLocaleInfo const formats[] =
		{wxLOCALE_SHORT_DATE_FMT, wxLOCALE_LONG_DATE_FMT};
	size_t const num_formats = sizeof(formats) / sizeof(formats[0]);
	assert (num_formats > 0);
	for (size_t i = 0; i != num_formats; ++i)
	{
		date_wx.ParseFormat
		(	date_text,
			locale().GetInfo(formats[i]),
			&parsed_to_position
		);
		if (parsed_to_position == date_text.end())
		{
			break;
		}
	}
	if (parsed_to_position == date_text.end())
	{
		// Parsing was successful
		int year = date_wx.GetYear();
		if (year < 100) year += 2000;
		int const month = static_cast<int>(date_wx.GetMonth()) + 1;
		int const day = date_wx.GetDay();
		try
		{
			m_date = gregorian::date(year, month, day);
			return true;
		}
		catch (boost::exception&)
		{
			// Cannot construct gregorian::date.
			// Fall through to produce error message below.
		}
	}
	wxMessageBox
	(	wxString("\"") +
		date_text +
		wxString("\" ") +
		wxString("is not recognised as a date.")
	);
	return false;
}

bool
DateValidator::TransferFromWindow()
{
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	return true;
}

bool
DateValidator::TransferToWindow()
{
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
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
		assert (m_allow_blank && !m_date);
		text_ctrl->SetValue(wxEmptyString);
		return true;
	}
	assert (m_date);
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

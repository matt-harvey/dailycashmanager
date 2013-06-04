// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "date_validator.hpp"
#include "date.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/exception/all.hpp>
#include <jewel/debug_log.hpp>
#include <wx/datetime.h>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include <wx/validate.h>
#include <cassert>
#include <iostream>

using std::endl;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

DateValidator::DateValidator(gregorian::date const& p_date):
	m_date(p_date)
{
}

DateValidator::DateValidator(DateValidator const& rhs):
	wxValidator(),
	m_date(rhs.m_date)
{
}

bool
DateValidator::Validate(wxWindow* WXUNUSED(parent))
{
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
	date_wx.ParseDate(date_text, &parsed_to_position);
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
	text_ctrl->SetValue(date_format_wx(m_date));
	return true;
}

wxObject*
DateValidator::Clone() const
{
	return new DateValidator(*this);
}

}  // namespace gui
}  // namespace phatbooks

#include "date_validator.hpp"
#include "date.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/debug_log.hpp>
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
	if (parsed_to_position != date_text.end())
	{
		// TODO Display error message
		JEWEL_DEBUG_LOG << "Date string no good!" << endl;
		return false;
	}
	JEWEL_DEBUG_LOG << "Year: " << date_wx.GetYear() << endl;
	JEWEL_DEBUG_LOG << "Month: " << static_cast<int>(date_wx.GetMonth()) + 1 << endl;
	JEWEL_DEBUG_LOG << "Day: " << date_wx.GetDay() << endl;
	int year = date_wx.GetYear();
	if (year < 100) year += 2000;
	int const month = static_cast<int>(date_wx.GetMonth()) + 1;
	int const day = date_wx.GetDay();
	m_date = gregorian::date(year, month, day);
	return true;
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

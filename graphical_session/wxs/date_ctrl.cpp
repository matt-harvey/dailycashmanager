#include "date_ctrl.hpp"
#include "date_validator.hpp"
#include "locale.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <wx/datetime.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(DateCtrl, wxTextCtrl)
	EVT_KILL_FOCUS(DateCtrl::on_kill_focus)
END_EVENT_TABLE()

DateCtrl::DateCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	wxSize const& p_size,
	gregorian::date const& p_date
):
	wxTextCtrl
	(	p_parent,
		p_id,
		date_format_wx(p_date),
		wxDefaultPosition,
		p_size,
		wxALIGN_RIGHT,
		DateValidator(p_date)
	),
	m_date(p_date)
{
}
	

boost::gregorian::date
DateCtrl::date() const
{
	// TODO Factor out code that is duplicated between here and
	// DateValidator::Validate.
	wxDateTime date_wx;
	wxString const date_text = GetValue();
	wxString::const_iterator parsed_to_position;
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
	if (parsed_to_position != date_text.end())
	{
		throw InvalidDateException
		(	"Cannot extract a valid wxDateTime from DateCtrl."
		);
	}
	int year = date_wx.GetYear();
	if (year < 100) year += 2000;
	int const month = static_cast<int>(date_wx.GetMonth()) + 1;
	int const day = date_wx.GetDay();
	try
	{
		return gregorian::date(year, month, day);
	}
	catch (boost::exception&)
	{
		throw InvalidDateException
		(	"Cannot convert wxDateTime to boost::gregorian::date."
		);
	}
}
	



void
DateCtrl::on_kill_focus(wxFocusEvent& event)
{
	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the DateCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
}

}  // namespace gui
}  // namespace phatbooks

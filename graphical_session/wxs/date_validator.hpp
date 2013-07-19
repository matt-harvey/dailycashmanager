// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_date_validator_hpp
#define GUARD_date_validator_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/validate.h>

namespace phatbooks
{
namespace gui
{

/**
 * Validates a wxString purporting to represent a date.
 */
class DateValidator: public wxValidator
{
public:
	DateValidator(boost::gregorian::date const& p_date, bool p_allow_blank);
	DateValidator(DateValidator const& rhs);

	/**
	 * @param parent should point to an instance of a wxTextCtrl.
	 */
	bool Validate(wxWindow* parent);

	bool TransferFromWindow();
	bool TransferToWindow();
	wxObject* Clone() const;

	boost::optional<boost::gregorian::date> date() const;

private:
	bool m_allow_blank;
	boost::optional<boost::gregorian::date> m_date;

};  // class DateValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_date_validator_hpp

#ifndef GUARD_date_validator_hpp
#define GUARD_date_validator_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <wx/validate.h>

namespace phatbooks
{
namespace gui
{

/**
 * Validates a wxString purporting to represent a date.
 *
 * @todo If this is no longer being used anywhere, get rid of it.
 */
class DateValidator: public wxValidator
{
public:
	DateValidator(boost::gregorian::date const& p_date);
	DateValidator(DateValidator const& rhs);

	/**
	 * @param parent should point to an instance of a wxTextCtrl.
	 */
	bool Validate(wxWindow* parent);

	bool TransferFromWindow();
	bool TransferToWindow();
	wxObject* Clone() const;

private:
	boost::gregorian::date m_date;

};  // class DateValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_date_validator_hpp

#ifndef GUARD_decimal_validator_hpp
#define GUARD_decimal_validator_hpp

#include <jewel/decimal.hpp>
#include <wx/intl.h>
#include <wx/valtext.h>



namespace phatbooks
{
namespace gui
{

class DecimalValidator: public wxTextValidator
{
public:
	DecimalValidator
	(	jewel::Decimal const& p_decimal,
		jewel::Decimal::places_type p_precision,
  		// Allow dash for zero when PRINTING (always allowed when reading)
		bool p_print_dash_for_zero = true
	);

	DecimalValidator(DecimalValidator const& rhs);

	/**
	 * @param parent should point to an instance of wxTextCtrl.
	 */
	bool Validate(wxWindow* parent);

	bool TransferFromWindow();
	bool TransferToWindow();
	wxObject* Clone() const;

private:
	bool m_print_dash_for_zero;
	jewel::Decimal::places_type m_precision;
	jewel::Decimal m_decimal;

};  // class DecimalValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_decimal_validator_hpp

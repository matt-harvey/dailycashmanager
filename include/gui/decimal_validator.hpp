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

#ifndef GUARD_decimal_validator_hpp_503944817233805
#define GUARD_decimal_validator_hpp_503944817233805

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

	DecimalValidator(DecimalValidator const&) = default;
	DecimalValidator(DecimalValidator&&) = default;
	DecimalValidator& operator=(DecimalValidator const&) = default;
	DecimalValidator& operator=(DecimalValidator&&) = default;
	~DecimalValidator() = default;

	/**
	 * @param parent should point to an instance of wxTextCtrl.
	 */
	bool Validate(wxWindow* parent) override;

	bool TransferFromWindow() override;
	bool TransferToWindow() override;
	wxObject* Clone() const override;

	jewel::Decimal decimal() const;
	
	void set_precision(jewel::Decimal::places_type p_precision);

private:
	bool m_print_dash_for_zero;
	jewel::Decimal::places_type m_precision;
	jewel::Decimal m_decimal;

};  // class DecimalValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_decimal_validator_hpp_503944817233805

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

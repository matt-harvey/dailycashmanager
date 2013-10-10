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


#include "gui/decimal_validator.hpp"
#include "finformat.hpp"
#include "gui/app.hpp"
#include "gui/locale.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/exception.hpp>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>

using jewel::Decimal;
using jewel::round;

namespace phatbooks
{
namespace gui
{

DecimalValidator::DecimalValidator
(	Decimal const& p_decimal,
	Decimal::places_type p_precision,
	bool p_print_dash_for_zero
):
	m_print_dash_for_zero(p_print_dash_for_zero),
	m_precision(p_precision),
	m_decimal(p_decimal)
{
}

Decimal
DecimalValidator::decimal() const
{
	return m_decimal;
}

void
DecimalValidator::set_precision(jewel::Decimal::places_type p_precision)
{
	m_precision = p_precision;
	m_decimal = round(m_decimal, p_precision);
	return;
}

bool
DecimalValidator::Validate(wxWindow* WXUNUSED(parent))
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl const* const text_ctrl =
		dynamic_cast<wxTextCtrl*>(GetWindow());
	if (!text_ctrl)
	{
		return false;
	}
	try
	{
		m_decimal = round
		(	wx_to_decimal(wxString(text_ctrl->GetValue()), locale()),
			m_precision
		);
		return true;
	}
	catch (jewel::Exception&)
	{
		try
		{
			Decimal const raw_sum =
				wx_to_simple_sum(wxString(text_ctrl->GetValue()), locale());
			m_decimal = round(raw_sum, m_precision);
			return true;
		}
		catch (jewel::Exception&)
		{
			// We don't display an error message here. We just return false,
			// without updating m_decimal.
			return false;
		}
	}
	JEWEL_HARD_ASSERT (false);
}

bool
DecimalValidator::TransferFromWindow()
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	return true;
}

bool
DecimalValidator::TransferToWindow()
{
	JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl* const text_ctrl = dynamic_cast<wxTextCtrl*>(GetWindow());
	if (!text_ctrl)
	{
		return false;
	}
	DecimalFormatFlags flags;
	if (!m_print_dash_for_zero) flags.clear(string_flags::dash_for_zero);
	text_ctrl->SetValue(finformat_wx(m_decimal, locale(), flags));
	return true;
}

wxObject*
DecimalValidator::Clone() const
{
	return new DecimalValidator(*this);
}



}  // namespace gui
}  // namespace phatbooks

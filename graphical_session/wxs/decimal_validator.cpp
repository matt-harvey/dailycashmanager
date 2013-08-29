// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "decimal_validator.hpp"
#include "app.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/exception.hpp>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <cassert>

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

DecimalValidator::DecimalValidator(DecimalValidator const& rhs):
	wxTextValidator(),
	m_print_dash_for_zero(rhs.m_print_dash_for_zero),
	m_precision(rhs.m_precision),
	m_decimal(rhs.m_decimal)
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
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
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
	assert (false);
}

bool
DecimalValidator::TransferFromWindow()
{
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	return true;
}

bool
DecimalValidator::TransferToWindow()
{
	assert (GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));
	wxTextCtrl* const text_ctrl = dynamic_cast<wxTextCtrl*>(GetWindow());
	if (!text_ctrl)
	{
		return false;
	}
	text_ctrl->
		SetValue(finformat_wx(m_decimal, locale(), m_print_dash_for_zero));
	return true;
}

wxObject*
DecimalValidator::Clone() const
{
	return new DecimalValidator(*this);
}



}  // namespace gui
}  // namespace phatbooks

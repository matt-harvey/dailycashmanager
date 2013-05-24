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
	Decimal::places_type p_precision
):
	m_precision(p_precision),
	m_decimal(p_decimal)
{
}

DecimalValidator::DecimalValidator(DecimalValidator const& rhs):
	wxTextValidator(),
	m_precision(rhs.m_precision),
	m_decimal(rhs.m_decimal)
{
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
	catch (jewel::Exception& e)
	{
		// TODO e.what() is not user-friendly enough.
		wxMessageBox(std8_to_wx(e.what()));
		return false;
	}
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
	text_ctrl->SetValue(finformat_wx(m_decimal, locale()));
	return true;
}

wxObject*
DecimalValidator::Clone() const
{
	return new DecimalValidator(*this);
}



}  // namespace gui
}  // namespace phatbooks
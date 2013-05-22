#include "decimal_validator.hpp"
#include "app.hpp"
#include "finformat.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/exception.hpp>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/validate.h>
#include <cassert>

using jewel::Decimal;

namespace phatbooks
{
namespace gui
{

DecimalValidator::DecimalValidator(Decimal const& p_decimal):
	m_decimal(p_decimal)
{
}

DecimalValidator::DecimalValidator(DecimalValidator const& rhs):
	wxValidator(),
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
		m_decimal = wx_to_decimal(wxString(text_ctrl->GetValue()), locale());
		return true;
	}
	catch (jewel::Exception& e)
	{
		// TODO Is e.what() user-friendly enough?
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


wxLocale const&
DecimalValidator::locale() const
{
	App* app = dynamic_cast<App*>(wxTheApp);
	return app->locale();
}

}  // namespace gui
}  // namespace phatbooks

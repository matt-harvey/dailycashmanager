// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "decimal_text_ctrl.hpp"
#include "decimal_validator.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "phatbooks_exceptions.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <iostream>

using jewel::Decimal;
using std::endl;

namespace phatbooks
{
namespace gui
{


BEGIN_EVENT_TABLE(DecimalTextCtrl, wxTextCtrl)
	EVT_KILL_FOCUS(DecimalTextCtrl::on_kill_focus)
END_EVENT_TABLE()


DecimalTextCtrl::DecimalTextCtrl
(	wxWindow* p_parent,
	unsigned int p_id,
	wxSize const& p_size,
	Decimal::places_type p_precision,
	bool p_print_dash_for_zero  // TODO Would be cleaner with a FlagSet here.
):
	wxTextCtrl
	(	p_parent,
		p_id,
		finformat_wx
		(	Decimal(0, p_precision),
			locale(),
			(	p_print_dash_for_zero?
				DecimalFormatFlags():
				DecimalFormatFlags().clear(string_flags::dash_for_zero)
			)
		),
		wxDefaultPosition,
		p_size,
		wxALIGN_RIGHT,
		DecimalValidator
		(	Decimal(0, p_precision),
			p_precision,
			p_print_dash_for_zero
		)
	),
	m_print_dash_for_zero(p_print_dash_for_zero),
	m_precision(p_precision)
{
}

DecimalTextCtrl::~DecimalTextCtrl()
{
}

void
DecimalTextCtrl::set_amount(Decimal const& p_amount)
{
	Decimal::places_type const prec = p_amount.places();
	if (prec != m_precision)
	{
		DecimalValidator* const validator =
			dynamic_cast<DecimalValidator*>(GetValidator());	
		JEWEL_ASSERT (validator);
		m_precision = prec;
		validator->set_precision(prec);
	}
	JEWEL_ASSERT (p_amount.places() == m_precision);
	DecimalFormatFlags flags;
	if (!m_print_dash_for_zero)
	{
		flags.clear(string_flags::dash_for_zero);
	}
	wxString const amount_string = finformat_wx
	(	p_amount,
		locale(),
		flags
	);
	SetValue(amount_string);

	// TODO This really sucks. We are validating the entire parent
	// window as a side-effect of setting the value of just one
	// of its children. But if we call Validate() etc.. on
	// DecimalTextCtrl directly it doesn't have any effect (for some
	// reason...).
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();

	return;
}

Decimal
DecimalTextCtrl::amount()
{
	DecimalValidator const* const validator =
		dynamic_cast<DecimalValidator const*>(GetValidator());
	JEWEL_ASSERT (validator);
	return validator->decimal();
}

void
DecimalTextCtrl::on_kill_focus(wxFocusEvent& event)
{
	do_on_kill_focus(event);
	return;
}

void
DecimalTextCtrl::do_on_kill_focus(wxFocusEvent& event)
{
	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the DecimalTextCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
}

}  // namespace gui
}  // namespace phatbooks

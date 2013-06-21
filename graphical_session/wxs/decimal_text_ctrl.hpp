// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_decimal_text_ctrl_hpp
#define GUARD_decimal_text_ctrl_hpp

#include <jewel/decimal.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

/**
 * A subclass of wxTextCtrl specifically for the entry and validation
 * of wxString representations of jewel::Decimal.
 */
class DecimalTextCtrl: public wxTextCtrl
{
public:
	DecimalTextCtrl
	(	wxWindow* p_parent,
		unsigned int p_id,
		wxSize const& p_size,
		jewel::Decimal::places_type p_precision,
		bool p_print_dash_for_zero = true
	);
	
	/**
	 * @throws phatbooks::PrecisionException if and only if
	 * p_amount.places() does not match the inherent precision of
	 * the EntryCtrl.
	 */
	void set_amount(jewel::Decimal const& p_amount);

	jewel::Decimal amount() const;

private:
	void on_kill_focus(wxFocusEvent& event);
	bool m_print_dash_for_zero;
	jewel::Decimal::places_type m_precision;
	DECLARE_EVENT_TABLE()

};  // class DecimalTextCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_decimal_text_ctrl_hpp

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
 *
 * @todo We are using DecimalValidator in the implementation of this.
 * But it seems like this is only creating complexity, without providing
 * any real benefit. Should I just move the behaviour we do require from
 * DecimalValidator directly into DecimalTextCtrl, and then abolish
 * DecimalValidator?
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

	virtual ~DecimalTextCtrl();

	/**
	 * WARNING As a side-effect of calling this function, the
	 * \e parent window of the DecimalTextCtrl will have
	 * Validate() and TransferDataFromWindow() called on it.
	 * This is an unfortunate workaround for odd behaviour whereby
	 * wWidgets does not seem to support these functions being
	 * called directly on the DecimalTextCtl itself.
	 */
	void set_amount(jewel::Decimal const& p_amount);

	jewel::Decimal amount();

private:
	void on_kill_focus(wxFocusEvent& event);
	virtual void do_on_kill_focus(wxFocusEvent& event);

	bool m_print_dash_for_zero;
	jewel::Decimal::places_type m_precision;
	DECLARE_EVENT_TABLE()

};  // class DecimalTextCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_decimal_text_ctrl_hpp

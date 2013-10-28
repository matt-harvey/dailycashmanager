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

#ifndef GUARD_decimal_text_ctrl_hpp_19653581837030865
#define GUARD_decimal_text_ctrl_hpp_19653581837030865

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

	DecimalTextCtrl(DecimalTextCtrl const&) = delete;
	DecimalTextCtrl(DecimalTextCtrl&&) = delete;
	DecimalTextCtrl& operator=(DecimalTextCtrl const&) = delete;
	DecimalTextCtrl& operator=(DecimalTextCtrl&&) = delete;
	virtual ~DecimalTextCtrl();

	/**
	 * NOTE As a side-effect of calling this function, the
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

#endif  // GUARD_decimal_text_ctrl_hpp_19653581837030865

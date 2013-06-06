#ifndef GUARD_transaction_type_ctrl_hpp
#define GUARD_transaction_type_ctrl_hpp

#include "transaction_type.hpp"
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <wx/spinctrl.h>

namespace phatbooks
{
namespace gui
{

/**
 * Widget to enable the user to select a TransactionType.
 *
 * @todo To implement this, we need to capture the EVT_SPINCTRL /
 * wxSpinEvent, and, on the basis of the underlying int/TransactionType
 * value, set the text that is shown in the control.
 */
class TransactionTypeCtrl: public wxSpinCtrl
{
public:
	TransactionTypeCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize p_size,
		transaction_type::TransactionType p_transaction_type =
			static_cast<transaction_type::TransactionType>(0)
	);

private:
	void on_spin(wxSpinEvent& event);
	transaction_type::TransactionType m_transaction_type;

	DECLARE_EVENT_TABLE()

};  // class TransactionTypeCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_type_ctrl_hpp

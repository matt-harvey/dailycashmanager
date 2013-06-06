#include "transaction_type_ctrl.hpp"
#include "b_string.hpp"
#include "transaction_type.hpp"
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <wx/spinctrl.h>
#include <algorithm>

using std::max;


// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;


namespace phatbooks
{

using transaction_type::num_transaction_types;
using transaction_type::TransactionType;

namespace gui
{

BEGIN_EVENT_TABLE(TransactionTypeCtrl, wxSpinCtrl)
	EVT_SPINCTRL(wxID_ANY, TransactionTypeCtrl::on_spin)
END_EVENT_TABLE()


TransactionTypeCtrl::TransactionTypeCtrl
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxSize p_size
):
	wxSpinCtrl
	(	p_parent,
		p_id,
		bstring_to_wx(transaction_type_to_verb(static_cast<TransactionType>(0))),
		wxDefaultPosition,
		p_size,
		wxSP_ARROW_KEYS,
		0,
		max(0, static_cast<int>(num_transaction_types) - 1),
		0
	)
{
}

void
TransactionTypeCtrl::on_spin(wxSpinEvent& event)
{
	JEWEL_DEBUG_LOG << "Entered TransactionTypeCtrl::on_spin(...)" << endl;
	TransactionType const new_transaction_type =
		static_cast<TransactionType>(event.GetPosition());
	JEWEL_DEBUG_LOG << "After spinning, TransactionType as int"
	                << static_cast<int>(new_transaction_type)
					<< endl;
	JEWEL_DEBUG_LOG << "After spinning, "
	                << "transaction_type_to_verb(m_transaction_type) is: "
	                << transaction_type_to_verb(new_transaction_type)
					<< endl;
	SetValue(bstring_to_wx(transaction_type_to_verb(new_transaction_type)));
	return;
}
	


}  // namespace gui
}  // namespace phatbooks

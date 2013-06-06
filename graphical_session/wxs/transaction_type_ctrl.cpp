#include "transaction_type_ctrl.hpp"
#include "b_string.hpp"
#include "transaction_type.hpp"
#include <algorithm>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <wx/spinctrl.h>

using std::max;

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
	wxSize p_size,
	TransactionType p_transaction_type
):
	wxSpinCtrl
	(	p_parent,
		p_id,
		bstring_to_wx(transaction_type_to_verb(p_transaction_type)),
		wxDefaultPosition,
		p_size,
		wxSP_ARROW_KEYS,
		0,
		static_cast<int>(max(0, num_transaction_types - 1))
	),
	m_transaction_type(p_transaction_type)
{
}

void
TransactionTypeCtrl::on_spin(wxSpinEvent& event)
{
	m_transaction_type = static_cast<TransactionType>(event.GetPosition());
	SetValue(bstring_to_wx(transaction_type_to_verb(m_transaction_type)));
	return;
}
	


}  // namespace gui
}  // namespace phatbooks

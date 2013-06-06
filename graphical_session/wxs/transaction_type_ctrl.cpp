#include "transaction_type_ctrl.hpp"
#include "b_string.hpp"
#include "string_set_validator.hpp"
#include "transaction_type.hpp"
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using std::vector;


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

BEGIN_EVENT_TABLE(TransactionTypeCtrl, wxComboBox)
	EVT_KILL_FOCUS(TransactionTypeCtrl::on_kill_focus)
END_EVENT_TABLE()


TransactionTypeCtrl::TransactionTypeCtrl
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxSize const& p_size
):
	wxComboBox
	(	p_parent,
		p_id,
		bstring_to_wx
		(	transaction_type_to_verb(static_cast<TransactionType>(0))
		),
		wxDefaultPosition,
		p_size,
		wxArrayString()
	)
{
	wxArrayString transaction_type_verbs;
	assert (transaction_type_verbs.IsEmpty());
	vector<TransactionType> const tt = transaction_types();
	vector<TransactionType>::const_iterator it = tt.begin();
	vector<TransactionType>::const_iterator const end = tt.end();
	for ( ; it != end; ++it)
	{
		wxString const verb = bstring_to_wx
		(	transaction_type_to_verb(*it)
		);
		transaction_type_verbs.Add(verb);  // remember as valid name
		Append(verb);  // add to combobox
	}
	StringSetValidator validator
	(	GetValue(),
		transaction_type_verbs,
		"transaction type"
	);
	SetValidator(validator);
	AutoComplete(transaction_type_verbs);
}

void
TransactionTypeCtrl::on_kill_focus(wxFocusEvent& event)
{
	// TODO Make a class from which we can privately inherit, to capture
	// this on_kill_focus behaviour, which is shared by several custom
	// widget classes in phatbooks::gui.

	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the AccountCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
}



}  // namespace gui
}  // namespace phatbooks

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "transaction_type_ctrl.hpp"
#include "b_string.hpp"
#include "string_set_validator.hpp"
#include "transaction_ctrl.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using boost::optional;
using jewel::value;
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
	EVT_KILL_FOCUS
	(	TransactionTypeCtrl::on_kill_focus
	)
	EVT_COMBOBOX
	(	wxID_ANY,
		TransactionTypeCtrl::on_change
	)
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
		wxArrayString(),
		wxCB_READONLY
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
	SetSelection(0);  // In effort to avoid apparent bug in Windows.
	// TODO AutoComplete is irrelevant if we have the wxComboBox as readonly.
	// But we still might want to let the user select by just typing the first
	// letter or something.
	AutoComplete(transaction_type_verbs);
}

optional<transaction_type::TransactionType>
TransactionTypeCtrl::transaction_type() const
{
	optional<transaction_type::TransactionType> ret;
	int const selection = GetSelection();
	if (selection < 0)
	{
		return ret;
	}
	transaction_type::TransactionType const ttype =
		static_cast<transaction_type::TransactionType>(selection);
	assert_transaction_type_validity(ttype);
	ret = ttype;
	return ret;
}

void
TransactionTypeCtrl::set_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	SetSelection(static_cast<int>(p_transaction_type));
	return;
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

void
TransactionTypeCtrl::on_change(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused param.
	TransactionCtrl* parent = dynamic_cast<TransactionCtrl*>(GetParent());
	assert(parent);
	optional<transaction_type::TransactionType> const maybe_ttype =
		transaction_type();
	if (!maybe_ttype)
	{
		return;
	}
	assert_transaction_type_validity(value(maybe_ttype));
	parent->refresh_for_transaction_type(value(maybe_ttype));
	return;
}


}  // namespace gui
}  // namespace phatbooks

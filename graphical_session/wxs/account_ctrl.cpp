#include "account_ctrl.hpp"
#include "account.hpp"
#include "b_string.hpp"
#include "string_set_validator.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/combobox.h>
#include <wx/event.h>
#include <cassert>

#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(AccountCtrl, wxComboBox)
	EVT_KILL_FOCUS(AccountCtrl::on_kill_focus)
END_EVENT_TABLE()

Account
AccountCtrl::account()
{
	StringSetValidator const* const validator =
		dynamic_cast<StringSetValidator const*>(GetValidator());
	assert (validator);
	return Account(m_database_connection, wx_to_bstring(GetValue()));
}

void
AccountCtrl::on_kill_focus(wxFocusEvent& event)
{
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

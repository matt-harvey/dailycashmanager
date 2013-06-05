#include "account_ctrl.hpp"
#include "account.hpp"
#include "account_name_validator.hpp"
#include "phatbooks_database_connection.hpp"
#include <wx/combobox.h>
#include <wx/event.h>
#include <cassert>

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
	AccountNameValidator const* const validator =
		dynamic_cast<AccountNameValidator const*>(GetValidator());
	assert (validator);
	return Account
	(	m_database_connection,
		validator->account_name()
	);
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

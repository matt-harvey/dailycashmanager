#ifndef GUARD_account_dialog_hpp
#define GUARD_account_dialog_hpp

#include "account_type.hpp"
#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>

namespace phatbooks
{

// Begin forward declarations

class Account;

namespace gui
{

class AccountTypeCtrl;
class DecimalTextCtrl;
class Frame;

// End forward declarations

/**
 * Dialog to facilitate creation of new Accounts and editing of details
 * of existing Accounts.
 */
class AccountDialog: public wxDialog, private boost::noncopyable
{
public:

	AccountDialog
	(	Frame* p_parent,
		Account& p_account,
		account_super_type::AccountSuperType p_account_super_type
	);

private:

	wxGridBagSizer* m_top_sizer;
	wxTextCtrl* m_name_ctrl;
	AccountTypeCtrl* m_account_type_ctrl;
	wxTextCtrl* m_description_ctrl;
	DecimalTextCtrl* m_opening_amount_ctrl;

	Account& m_account;

};  // class AccountDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_dialog_hpp

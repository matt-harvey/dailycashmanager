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

	/**
	 * @param p_parent parent Frame.
	 *
	 * @param p_account the Account the editing and possible saving of which
	 * will be facilitated via this widget. If p_account does not have an
	 * id, then it is assumed that none of its other settable characteristics
	 * have been set, and the AccountDialog will be used to populate the
	 * Account's details from scratch. If p_account does have an id, then
	 * it is assumed that all of its settable characteristics have already
	 * been set. In that case, the AccountDialog will be used as a means
	 * for the user optionally to change some or all of the existing
	 * characteristics of the Account.
	 */
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

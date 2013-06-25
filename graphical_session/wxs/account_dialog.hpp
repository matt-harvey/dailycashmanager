#ifndef GUARD_account_dialog_hpp
#define GUARD_account_dialog_hpp

#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

namespace phatbooks
{

// Begin forward declarations

class Account;

namespace gui
{

class Frame;

// End forward declarations

/**
 * Dialog to facilitate creation of new Accounts and editing of details
 * of existing Accounts.
 */
class AccountDialog: public wxDialog, private boost::noncopyable
{
public:

	AccountDialog(Frame* p_parent, Account& p_account);

private:

	Account& m_account;

};  // class AccountDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_dialog_hpp

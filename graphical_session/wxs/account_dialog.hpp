#ifndef GUARD_account_dialog_hpp
#define GUARD_account_dialog_hpp

#include "account_type.hpp"
#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

class Account;

namespace gui
{

class AccountTypeCtrl;
class DecimalTextCtrl;

// End forward declarations

/**
 * Dialog to facilitate creation of new Accounts and editing of details
 * of existing Accounts.
 *
 * @todo We don't want the user to be able to choose the Equity
 * AccountType. Also, we don't want the user to be able to change a
 * P&L Account to account_type::pure_envelope if it has actual
 * transactions recorded against it. In fact, if the user is editing
 * an existing Account, it probably doesn't make a lot of sense to allow
 * them to change the existing AccountType at all. It would be only very
 * rarely that the user would want to change this anyway; enabling them
 * to change it may just be confusing.
 *
 * @todo The concept of "Opening balance" could be quite confusing for
 * the user. The "Opening balance", on its current "business logic" meaning,
 * refers to the balance of an Account on the entity start date. However,
 * the user may think it is referring to the opening deposit into the
 * Account. We need to clarify this for the user. Either incorporate the
 * "opening deposit" concept into the AccountDialog, or else make it clear
 * to the user that "Opening balance" refers to balance at entity start
 * date.
 *
 * @todo HIGH PRIORITY Make it so the user cannot proceed if there are
 * duplicate Account names (tested case-insensitively).
 *
 * @todo Make it so that the user can't change the AccountSuperType of an
 * existing Account.
 *
 * @todo Make it so that when setting up a new Account, if the user
 * changes AccountSuperType, it causes the BudgetDialog part of the
 * controls to appear or disappear accordingly.
 */
class AccountDialog: public wxDialog, private boost::noncopyable
{
public:

	/**
	 * @param p_parent parent Window. <em>Should not be null.</em>
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
	(	wxWindow* p_parent,
		Account& p_account,
		account_super_type::AccountSuperType p_account_super_type
	);

private:
	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	void configure_bottom_controls();
	void configure_buttons();

	account_super_type::AccountSuperType account_super_type() const;

	/**
	 * Updates m_account based on the data entered by the user.
	 *
	 * @param p_is_new_account should be passed true if and only if
	 * m_account does not have an id.
	 *
	 * @returns true if and only if m_account is successfully updated
	 * and saved.
	 */
	bool update_account_from_dialog(bool p_is_new_account);

	int m_current_row;
	wxGridBagSizer* m_top_sizer;
	wxTextCtrl* m_name_ctrl;
	AccountTypeCtrl* m_account_type_ctrl;
	wxTextCtrl* m_description_ctrl;
	DecimalTextCtrl* m_opening_amount_ctrl;
	wxButton* m_cancel_button;
	wxButton* m_ok_button;

	Account& m_account;

	DECLARE_EVENT_TABLE()

};  // class AccountDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_dialog_hpp

/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GUARD_account_dialog_hpp_4068064533205733
#define GUARD_account_dialog_hpp_4068064533205733

#include "account.hpp"
#include "account_type.hpp"
#include "budget_panel.hpp"
#include <sqloxx/handle.hpp>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace phatbooks
{

// Begin forward declarations

namespace gui
{

class AccountTypeCtrl;
class DecimalTextCtrl;

// End forward declarations

/**
 * Dialog to facilitate creation of new Accounts and editing of details
 * of existing Accounts.
 *
 * @todo LOW PRIORITY Enable user to delete an Account, but only if it's one for
 * which no Entries have been made, and for which the opening balance
 * is nil, and where, after the proposed deletion, it will still be
 * the case that there is at least one balance sheet Account, at least
 * one revenue Account and at least one expense Account (because this
 * will ensure all TransactionTypes are still available). If and when I
 * enable this, I will need to implement update_for_deleted(sqloxx::Id)
 * for all relevant widget classes, including AccountCtrl.
 */
class AccountDialog: public wxDialog
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
		sqloxx::Handle<Account> const& p_account,
		AccountSuperType p_account_super_type
	);

	AccountDialog(AccountDialog const&) = delete;
	AccountDialog(AccountDialog&&) = delete;
	AccountDialog& operator=(AccountDialog const&) = delete;
	AccountDialog& operator=(AccountDialog&&) = delete;
	virtual ~AccountDialog();

private:
	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	void configure_budget_panel();
	void configure_bottom_row();

	AccountSuperType account_super_type() const;

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
	BudgetPanel* m_budget_panel;
	wxCheckBox* m_visibility_ctrl;
	wxButton* m_cancel_button;
	wxButton* m_ok_button;
	sqloxx::Handle<Account> m_account;

	DECLARE_EVENT_TABLE()

};  // class AccountDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_account_dialog_hpp_4068064533205733

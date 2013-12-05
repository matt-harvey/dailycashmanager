/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_account_dialog_hpp_4068064533205733
#define GUARD_account_dialog_hpp_4068064533205733

#include "account.hpp"
#include "account_type.hpp"
#include "budget_panel.hpp"
#include <sqloxx/handle.hpp>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/window.h>

namespace dcm
{

// Begin forward declarations

namespace gui
{

class AccountTypeCtrl;
class Button;
class DecimalTextCtrl;
class TextCtrl;

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

	AccountType selected_account_type() const;
	
private:
	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	void configure_budget_panel();
	void configure_bottom_row();

	AccountSuperType account_super_type() const;

	int m_current_row;
	wxGridBagSizer* m_top_sizer;
	TextCtrl* m_name_ctrl;
	AccountTypeCtrl* m_account_type_ctrl;
	TextCtrl* m_description_ctrl;
	DecimalTextCtrl* m_opening_amount_ctrl;
	BudgetPanel* m_budget_panel;
	wxCheckBox* m_visibility_ctrl;
	Button* m_cancel_button;
	Button* m_ok_button;
	sqloxx::Handle<Account> m_account;

	DECLARE_EVENT_TABLE();

};  // class AccountDialog


}  // namespace gui
}  // namespace dcm

#endif  // GUARD_account_dialog_hpp_4068064533205733

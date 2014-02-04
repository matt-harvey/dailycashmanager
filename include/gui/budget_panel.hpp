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

#ifndef GUARD_budget_panel_hpp_749080240143966
#define GUARD_budget_panel_hpp_749080240143966

#include "account.hpp"
#include "budget_item.hpp"
#include "decimal_text_ctrl.hpp"
#include "frequency_ctrl.hpp"
#include <boost/optional.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/window.h>
#include <vector>

namespace dcm
{

// Begin forward declarations

class DcmDatabaseConnection;

namespace gui
{

class AccountCtrl;
class AccountDialog;
class Button;
class DecimalTextCtrl;
class TextCtrl;

// End forward declarations


/**
 * Dialog for user to configure BudgetItems for a given Account.
 *
 * @todo MEDIUM PRIORITY Move the "daily top-up summary" down to be on the same
 * row as "Cancel" and "Save" buttons... but these buttons are actually
 * not in BudgetPanel, but are in the parent AccountDialog. So this
 * will require some careful refactoring to achieve.
 *
 * @todo MEDIUM PRIORITY There is some messiness in the implementation of this
 * class, especially to do with how the BudgetPanel responds when a
 * FrequencyCtrl or DecimalTextCtrl is updated. There is scope for improvement,
 * but it would probably require a ground-up refactor.
 */
class BudgetPanel: public wxPanel
{
public:

	/**
	 * @param p_parent parent window. <em>This should not be null.</em>
	 *
	 * @param p_account Handle to the Account for which the user is configuring
	 * BudgetItems.
	 *
	 * @throws BudgetEditingException if p_account is the balancing_account()
	 * of p_account.database_connection() - as the user should not be enabled
	 * to edit the balancing account's budget directly.
	 *
	 * \e Precondition: \e p_account must have an AccountType.
	 */
	BudgetPanel
	(	AccountDialog* p_parent,
		sqloxx::Handle<Account> const& p_account
	);

	BudgetPanel(BudgetPanel const&) = delete;
	BudgetPanel(BudgetPanel&&) = delete;
	BudgetPanel& operator=(BudgetPanel const&) = delete;
	BudgetPanel& operator=(BudgetPanel&&) = delete;
	virtual ~BudgetPanel();

	/**
	 * \e Precondition: \e m_account must have an ID before this is called.
	 */
	bool process_confirmation();

private:

	void revert_dialog_to_budgets();

	void on_pop_item_button_click(wxCommandEvent& event);
	void on_push_item_button_click(wxCommandEvent& event);

	/**
	 * This is triggered by DecimalTextCtrl::on_kill_focus(...).
	 * We override with behaviour we need to set budget summary
	 * text.
	 */
	virtual bool TransferDataToWindow() override;

	/**
	 * Updates budget summary text at top of Dialog, on the basis
	 * the BudgetItems passed to p_budget_items, regardless
	 * of what is in the database. The budget summary text encompasses
	 * both the amount and frequency.
	 */
	void update_budget_summary
	(	std::vector<sqloxx::Handle<BudgetItem> > const& p_budget_items
	);

	/**
	 * Updates budget summary text at top of Dialog, on the basis
	 * of what is currently shown in the GUI, regardless of what is
	 * in the database.
	 */
	void update_budget_summary();

	/**
	 * Updates the BudgetItems for m_account based on the data entered by
	 * the user, but does not save the changes to database.
	 *
	 * @returns true if and only if the BudgetItems for m_account are
	 * successfully updated.
	 */
	void update_budgets_from_dialog_without_saving();
	
	/**
	 * Updates the BudgetItems for m_account based on the data entered
	 * by the user, and saves changes to database.
	 *
	 * Precondition: \e m_account must have an Id before this is called.
	 */
	void update_budgets_from_dialog();

	/**
	 * Updates m_pop_item_button and/or m_push_item_button depending on
	 * whether it makes sense for these to be enabled or disabled.
	 */
	void update_button_disabledness();

	/**
	 * p_budget_item should have all attributes initialized, except that
	 * it need not have an id.
	 *
	 * Add a BudgetItemComponent describing p_budget_item. This does \e not
	 * affect the underlying BudgetItems (to update these call
	 * update_budgets_from_dialog()).
	 */
	void push_item_component(sqloxx::Handle<BudgetItem> const& p_budget_item);

	/**
	 * Remove the last BudgetItemComponent and update the budget summary text
	 * accordingly. This does \e affect the underlying BudgetItems (to update
	 * these call update_budgets_from_dialog()).
	 */
	void pop_item_component();

	DcmDatabaseConnection& database_connection() const;

	jewel::Decimal zero() const;

	static int const s_pop_item_button_id = wxID_HIGHEST + 1;
	static int const s_push_item_button_id = s_pop_item_button_id + 1;

	/**
	 * @returns string describing the standardized budget frequency for
	 * database_connection() on the basis of what is currently saved in the
	 * \e database, regardless of what is currently shown in the BudgetPanel
	 * itself.
	 */
	wxString initial_summary_amount_text();

	/**
	 * Like FrequencyCtrl, but change of selection causes parent
	 * BudgetPanel to update its summary text.
	 */
	class SpecialFrequencyCtrl: public FrequencyCtrl
	{
	public:
		SpecialFrequencyCtrl
		(	BudgetPanel* p_parent,
			wxWindowID p_id,
			wxSize const& p_size,
			DcmDatabaseConnection& p_database_connection
		);
	private:	
		void on_text_change(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
	};
	friend class SpecialFrequencyCtrl;

	/**
	 * Groups together into a single object a set of widgets
	 * representing a single BudgetItem.
	 */
	struct BudgetItemComponent
	{
		BudgetItemComponent():
			description_ctrl(nullptr),
			amount_ctrl(nullptr),
			frequency_ctrl(nullptr)
		{
		}
		TextCtrl* description_ctrl;
		DecimalTextCtrl* amount_ctrl;
		SpecialFrequencyCtrl* frequency_ctrl;
	};

	/**
	 * Used to warn user if signs are the wrong way round to
	 * what is expected for individual BudgetItems. Gives user
	 * the opportunity to correct the sign.
	 *
	 * Should only be used with AccountType::revenue and
	 * AccountType::expense. These are the only AccountTypes
	 * for which a particular sign is expected.
	 */
	class SignWarning: public wxMessageDialog
	{
	public:
		SignWarning
		(	wxWindow* p_parent,
			AccountType p_account_type
		);
	private:
		static wxString get_message(AccountType p_account_type);
	};

	/**
	 * Used to prompt the user for an offsetting budget adjustment to
	 * an Account other than the one whose BudgetItems have just been
	 * edited, to encourage balance budget.
	 */
	class BalancingDialog: public wxDialog
	{
	public:
		/**
		 * @param p_parent parent window.
		 *
		 * @param p_imbalance amount of the budget imbalance for which
		 * we will ask the user if they want to offset it elsewhere,
		 * expressed in terms of database_connection().budget_frequency().
		 *
		 * @param p_maybe_target If initialized, this is the Account
		 * which we will provide the user as the "default" suggested
		 * Account where they might like to offset the imbalance. If
		 * not initialized, then the "default" will just be whatever
		 * Account appears first in the AccountCtrl that will be
		 * provided within the BalancingDialog.
		 *
		 * Precondition: there must be at least one user-saved P&L Account
		 * saved in p_database_connection.
		 */
		BalancingDialog
		(	wxWindow* p_parent,
			jewel::Decimal const& p_imbalance,
			boost::optional<sqloxx::Handle<Account> > const& p_maybe_target,
			DcmDatabaseConnection& p_database_connection
		);
	private:
		void on_no_button_click(wxCommandEvent& event);
		void on_yes_button_click(wxCommandEvent& event);

		void update_budgets_from_dialog
		(	sqloxx::Handle<Account> const& p_account
		);

		bool budget_is_balanced() const;
		wxGridBagSizer* m_top_sizer;
		AccountCtrl* m_account_ctrl;
		Button* m_no_button;
		Button* m_yes_button;
		jewel::Decimal const m_imbalance;
		DcmDatabaseConnection& m_database_connection;
		DECLARE_EVENT_TABLE()
	};

	/**
	 * @returns a vector of handles to newly created BudgetItems (WITHOUT ids),
	 * based on the data currently in the BudgetItemComponent.
	 */
	std::vector<sqloxx::Handle<BudgetItem> > make_budget_items() const;

	/**
	 * Prompts user to select another Account to which to balance
	 * the budget; but does nothing if the budget now balances.
	 *
	 * @returns true if nothing went wrong.
	 */
	bool prompt_to_balance();
	
	std::vector<BudgetItemComponent> m_budget_item_components;
	std::vector<sqloxx::Handle<BudgetItem> > m_budget_items;
	size_t m_next_row;
	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_summary_label;
	wxStaticText* m_summary_amount_text;
	Button* m_pop_item_button;
	Button* m_push_item_button;
	sqloxx::Handle<Account> const& m_account;

	DECLARE_EVENT_TABLE()

};  // class BudgetPanel


}  // namespace gui
}  // namespace dcm

#endif  // GUARD_budget_panel_hpp_749080240143966

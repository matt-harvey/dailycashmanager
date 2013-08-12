// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_panel_hpp
#define GUARD_budget_panel_hpp

#include "account.hpp"
#include "budget_item.hpp"
#include "frequency_ctrl.hpp"
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class AccountDialog;
class DecimalTextCtrl;

// End forward declarations


/**
 * Dialog for user to configure BudgetItems for a given Account.
 *
 * @todo Make the "-" button look disabled unless there is more than
 * one BudgetItemComponent.
 *
 * @todo Improve the appearance of BudgetPanel, especially as it appears
 * within AccountDialog. Possibly move m_summary_text to a more "squared"
 * location within the window.
 */
class BudgetPanel: public wxPanel, private boost::noncopyable
{
public:

	/**
	 * @param p_parent parent window. <em>This should not be null.</em>
	 *
	 * @throws BudgetEditingException if p_account is the balancing_account()
	 * of p_account.database_connection() - as the user should not be enabled
	 * to edit the balancing account's budget directly.
	 *
	 * \e Precondition: \e p_account must have an AccountType.
	 */
	BudgetPanel(AccountDialog* p_parent, Account const& p_account);

	/**
	 * \e Precondition: \e m_account must have an ID before this is called.
	 */
	bool process_confirmation();

private:
	void on_pop_item_button_click(wxCommandEvent& event);
	void on_push_item_button_click(wxCommandEvent& event);

	/**
	 * This is triggered by DecimalTextCtrl::on_kill_focus(...).
	 * We override with behaviour we need to set budget summary
	 * text. WARNING How dodgy and indirect is this?
	 *
	 * @todo We also need SpecialFrequencyCtrl to trigger this...
	 */
	virtual bool TransferDataToWindow();

	/**
	 * Updates budget summary text at top of Dialog, on the basis
	 * what is currently shown in the BudgetPanel itself, regardless
	 * of what is in the database. The budget summary text encompasses
	 * both the amount and frequency.
	 */
	void update_budget_summary();

	/**
	 * Updates the BudgetItems for m_account based on the data entered
	 * by the user, and saves changes to database.
	 *
	 * @returns true if an only if the BudgetItems for m_account are
	 * successfully updated and saved.
	 *
	 * \e Precondition: \e m_account must have an id before this is called.
	 */
	bool update_budgets_from_dialog();

	/**
	 * p_budget_item should have all attributes initialized, except that
	 * it need not have an id.
	 *
	 * Add a BudgetItemComponent describing p_budget_item. This does \e not
	 * affect the underlying BudgetItems (to update these call
	 * update_budgets_from_dialog()).
	 */
	void push_item_component(BudgetItem const& p_budget_item);

	/**
	 * Remove the last BudgetItemComponent and update the budget summary text
	 * accordingly. This does \e affect the underlying BudgetItems (to update
	 * these call update_budgets_from_dialog()).
	 */
	void pop_item_component();

	PhatbooksDatabaseConnection& database_connection() const;

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
	 * @returns string describing the budget amount for m_account on the basis
	 * of what is currently saved in the \e database, regardless of what
	 * is currently shown in the BudgetPanel itself.
	 */
	wxString initial_summary_frequency_text();

	size_t m_next_row;

	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_summary_amount_text;
	wxStaticText* m_summary_frequency_text;
	wxButton* m_pop_item_button;
	wxButton* m_push_item_button;

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
			PhatbooksDatabaseConnection& p_database_connection
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
		wxTextCtrl* description_ctrl;
		DecimalTextCtrl* amount_ctrl;
		SpecialFrequencyCtrl* frequency_ctrl;
	};

	/**
	 * Used to warn user if signs are the wrong way round to
	 * what is expected for individual BudgetItems. Gives user
	 * the opportunity to correct the sign.
	 *
	 * Should only be used with account_type::revenue and
	 * account_type::expense. These are the only AccountTypes
	 * for which a particular sign is expected.
	 */
	class SignWarning: public wxMessageDialog
	{
	public:
		SignWarning
		(	wxWindow* p_parent,
			account_type::AccountType p_account_type
		);
	private:
		static wxString get_message(account_type::AccountType p_account_type);
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
			boost::optional<Account> const& p_maybe_target,
			PhatbooksDatabaseConnection& p_database_connection
		);
	private:
		void on_no_button_click(wxCommandEvent& event);
		void on_yes_button_click(wxCommandEvent& event);
		void update_budgets_from_dialog(Account& p_account);
		bool budget_is_balanced() const;
		wxGridBagSizer* m_top_sizer;
		AccountCtrl* m_account_ctrl;
		wxButton* m_no_button;
		wxButton* m_yes_button;
		jewel::Decimal const m_imbalance;
		PhatbooksDatabaseConnection& m_database_connection;
		DECLARE_EVENT_TABLE()
	};

	/**
	 * @returns a vector of newly created BudgetItems (WITHOUT ids),
	 * based on the data currently in the BudgetItemComponent.
	 */
	std::vector<BudgetItem> make_budget_items() const;

	/**
	 * Prompts user to select another Account to which to balance
	 * the budget; but does nothing if the budget now balances.
	 */
	void prompt_to_balance();
	
	std::vector<BudgetItemComponent> m_budget_item_components;

	Account const& m_account;
	std::vector<BudgetItem> m_budget_items;

	DECLARE_EVENT_TABLE()

};  // class BudgetPanel


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_budget_panel_hpp

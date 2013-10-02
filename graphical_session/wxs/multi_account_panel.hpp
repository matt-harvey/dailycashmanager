// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_multi_account_panel_hpp_2678521416465995
#define GUARD_multi_account_panel_hpp_2678521416465995

#include "account_handle.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "commodity_handle.hpp"
#include "decimal_text_ctrl.hpp"
#include "gridded_scrolled_panel.hpp"
#include "setup_wizard.hpp"
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <vector>

// for debugging
	#include <jewel/log.hpp>
	#include <iostream>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// end forward declarations

/**
 * An instance of this class is to be included in the
 * SetupWizard::BalanceSheetAccountPage. This class represents a scrolled
 * panel in which the user can create and configure multiple Accounts,
 * with the types, descriptions and opening balances.
 *
 * @todo We need to ensure that a the database connection has a default
 * Commodity, or else, we need to pass a Commodity separately to the
 * MultiAccountPanel constructor.
 *
 * @param Is the minimum number of rows such that the user will be
 * prevented from reducing the number of row to less than this number.
 * (Each row represents an Account.)
 */
class MultiAccountPanel: public GriddedScrolledPanel
{
public:
	MultiAccountPanel
	(	SetupWizard::AccountPage* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		AccountSuperType p_account_super_type,
		CommodityHandle const& p_commodity,
		size_t p_minimum_num_rows
	);
	
	MultiAccountPanel(MultiAccountPanel const&) = delete;
	MultiAccountPanel(MultiAccountPanel&&) = delete;
	MultiAccountPanel& operator=(MultiAccountPanel const&) = delete;
	MultiAccountPanel& operator=(MultiAccountPanel&&) = delete;
	virtual ~MultiAccountPanel() = default;

	/**
	 * @returns the width required to fit all the widgets in a
	 * MultiAccountPanel, along with a standard border and scrollbar.
	 */
	static int required_width();

	void set_commodity(CommodityHandle const& p_commodity);

	/**
	 * Populates \e out with AugmentedAccounts corresponding to what has
	 * been entered in the MultiAccountPanel by the user. The
	 * Accounts in the AugmentedAccounts will not have been saved (so
	 * will not have IDs). Also, no opening balance Journals will be saved
	 * (it is the client's responsibility to create opening balance Journals,
	 * if they desire to do so, using the information provided in the
	 * AugmentedAccounts. The Accounts in the AugmentedAccounts will have
	 * their commodity() attribute initialized to
	 * database_connection().default_commodity().
	 *
	 * \e out need not be empty when passed to this function. The selected
	 * AugmentedAccounts will simply be pushed onto the back of the existing
	 * vector, whether it is empty or not.
	 */
	void selected_augmented_accounts
	(	std::vector<AugmentedAccount>& out
	);

	/**
	 * @return \e true if and only if the Account names entered in the
	 * MultiAccountPanel are all unique (i.e. different from each other)
	 * on a case-insensitive basis, and are all non-blank. If the return
	 * value is \e false, then an user-friendly error message will be
	 * placed inside \e error_message.
	 */
	bool account_names_valid(wxString& p_error_message) const;

	/**
	 * @returns the total of all the amounts entered in the
	 * amount boxes in the MultiAccountPanel.
	 */
	jewel::Decimal total_amount() const;

	/**
	 * Add a new row with blank text boxes, and zero (rounded to precision
	 * of the Commodity of the MultiAccountPanel) in the opening balance
	 * boxes.
	 * 
	 * @returns \e true (assuming no exeption thrown) (never returns false).
	 * This returns a truth value rather than \e void, for consistency with
	 * \e pop_row().
	 */
	bool push_row();

	/**
	 * Removes the bottom Account row and returns true, provided the
	 * minimum number of rows would not be maintained; otherwise, does
	 * nothing and returns false.
	 */
	bool pop_row();

	void update_summary();

	size_t num_rows() const;

	/**
	 * @returns \e true if and only if the user has selected this
	 * AccountType in at least one row (which may or may not be a row in
	 * which a non-empty Account name has been entered).
	 */
	bool account_type_is_selected
	(	AccountType p_account_type
	) const;
	
private:

	jewel::Decimal summary_amount() const;

	class SpecialDecimalTextCtrl: public DecimalTextCtrl
	{
	public:
		SpecialDecimalTextCtrl
		(	MultiAccountPanel* p_parent,
			wxSize const& p_size
		);
		virtual ~SpecialDecimalTextCtrl();
	private:
		void do_on_kill_focus(wxFocusEvent& event) override;
	};

	friend class SpecialDecimalTextCtrl;

	/**
	 * @returns a handle to a newly created Account with Commodity not set but
	 * with text fields set to the empty string. The Account
	 * not have been saved to the database (so will not have an ID).
	 * Account AccountType will be set to an AccountType belonging to
	 * \e m_account_super_type.
	 */
	AccountHandle blank_account();

	/**
	 * Add a row to the display showing details for p_account. Be warned
	 * that this will set \e p_account.commodity() to m_commodity, even
	 * if \e p_account already has a Commodity set. Always returns \e true
	 * (returns \e bool for consistency with \e pop_row()).
	 */
	bool push_row(AccountHandle const& p_account);

	template <typename T> void pop_widget_from(std::vector<T>& p_vec);
	
	AccountSuperType m_account_super_type;
	CommodityHandle m_commodity;

	wxStaticText* m_summary_amount_text;
	
	size_t m_minimum_num_rows;

	std::vector<wxTextCtrl*> m_account_name_boxes;
	std::vector<AccountTypeCtrl*> m_account_type_boxes;
	std::vector<wxTextCtrl*> m_description_boxes;
	std::vector<SpecialDecimalTextCtrl*> m_opening_balance_boxes;

};  // class MultiAccountPanel


// IMPLEMENT MEMBER TEMPLATES

template <typename T>
void
MultiAccountPanel::pop_widget_from(std::vector<T>& p_vec)
{
	T doomed_elem = p_vec.back();
	top_sizer().Detach(doomed_elem);
	doomed_elem->Destroy();
	doomed_elem = 0;
	p_vec.pop_back();
	return;
}


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_multi_account_panel_hpp_2678521416465995

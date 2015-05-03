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

#ifndef GUARD_multi_account_panel_hpp_2678521416465995
#define GUARD_multi_account_panel_hpp_2678521416465995

#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "commodity.hpp"
#include "gridded_scrolled_panel.hpp"
#include "setup_wizard.hpp"
#include "opening_balance_ctrl.hpp"
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <set>
#include <vector>

namespace dcm
{

// begin forward declarations

class Account;
struct AugmentedAccount;
class DcmDatabaseConnection;

namespace gui
{

class Button;
class CheckBox;
class TextCtrl;

// end forward declarations

/**
 * An instance of this class is to be included in the
 * SetupWizard::BalanceSheetAccountPage. This class represents a scrolled
 * panel in which the user can create and configure multiple Accounts,
 * with the types, descriptions and opening balances.
 *
 * When first constructed, the MultiAccountPanel will contain a number
 * of rows some or all of which will contain data for creating default
 * "suggested" Accounts.
 *
 * @param Is the minimum number of rows such that the user will be
 * prevented from reducing the number of row to less than this number.
 * (Each row represents an Account.)
 */
class MultiAccountPanel: public GriddedScrolledPanel
{
public:
    MultiAccountPanel
    (   SetupWizard::AccountPage* p_parent,
        wxSize const& p_size,
        DcmDatabaseConnection& p_database_connection,
        AccountSuperType p_account_super_type,
        sqloxx::Handle<Commodity> const& p_commodity,
        size_t p_minimum_num_rows
    );
    
    MultiAccountPanel(MultiAccountPanel const&) = delete;
    MultiAccountPanel(MultiAccountPanel&&) = delete;
    MultiAccountPanel& operator=(MultiAccountPanel const&) = delete;
    MultiAccountPanel& operator=(MultiAccountPanel&&) = delete;
    virtual ~MultiAccountPanel();

    /**
     * @returns the width required to fit all the widgets in a
     * MultiAccountPanel, along with a standard border and scrollbar.
     */
    static int required_width();

    void set_commodity(sqloxx::Handle<Commodity> const& p_commodity);

    /**
     * @returns a vector populated with AugmentedAccounts corresponding to what
     * has been entered in the MultiAccountPanel by the user. The Accounts in
     * the AugmentedAccounts will not have been saved (so will not have IDs).
     * Also, no opening balance Journals will be saved (it is the client's
     * responsibility to create opening balance Journals, if they desire to do
     * so, using the information provided in the AugmentedAccounts. The
     * Accounts in the AugmentedAccounts will have their commodity() attribute
     * initialized to database_connection().default_commodity().
     */
    std::vector<AugmentedAccount> augmented_accounts();

    std::set<wxString> selected_account_names() const;
    
    /**
     * @return \e true if and only if the Account names entered in the
     * MultiAccountPanel are all unique (i.e. different from each other)
     * on a case-insensitive basis, and are all non-blank. If the return
     * value is \e false, then an user-friendly error message will be
     * placed inside \e error_message.
     */
    bool account_names_valid(wxString& p_error_message) const;

    /**
     * Inform the MultiAccountPanel that the Account names in the given
     * have already been used. The MultiAccountPanel will then take these
     * into account when validating candidate names for new Accounts.
     */
    void set_account_names_already_taken
    (   std::set<wxString> const& p_account_names_already_taken
    );

    /**
     * @returns the total of all the amounts entered in the
     * amount boxes in the MultiAccountPanel.
     */
    jewel::Decimal total_amount() const;

    /**
     * Add a new row with blank text boxes, and zero (rounded to precision
     * of the Commodity of the MultiAccountPanel) in the opening balance
     * boxes.
     */
    void push_row();

    /**
     * Removes the checked Account rows (those with checkboxes selected),
     * and returns true, provided the minimum number of rows would still be
     * maintained; otherwise, does nothing and returns false. If no rows are
     * checked, it returns true but does nothing.
     */
    bool remove_checked_rows();

    void update_summary();

    size_t num_rows() const;
    
    size_t num_checked_rows() const;

    /**
     * @returns \e true if and only if the user has selected this
     * AccountType in at least one row (which may or may not be a row in
     * which a non-empty Account name has been entered).
     */
    bool account_type_is_selected(AccountType p_account_type)const;

    /**
     * Returns the AccountType that is currently set for the row in
     * which OpeningBalanceCtrl is situated.
     *
     * Precondition: \e p_ctrl must have this MultiAccountPanel as its
     * parent.
     */
    AccountType account_type_for(OpeningBalanceCtrl const* p_ctrl) const;
    
private:

    virtual bool TransferDataToWindow() override;

    jewel::Decimal summary_amount() const;

    /**
     * @returns a handle to a newly created Account with Commodity not set but
     * with text fields set to the empty string. The Account will
     * not have been saved to the database (so will not have an ID).
     * Account AccountType will be set to an AccountType belonging to
     * \e m_account_super_type.
     */
    sqloxx::Handle<Account> blank_account();

    /**
     * Add a row to the display showing details for p_account. Be warned
     * that this will set \e p_account.commodity() to m_commodity, even
     * if \e p_account already has a Commodity set.
     */
    void push_row(sqloxx::Handle<Account> const& p_account);

    template <typename T> void remove_widgets_from(std::vector<T>& p_vec);

    std::set<std::vector<CheckBox*>::size_type> checked_rows() const;

    AccountSuperType m_account_super_type;
    sqloxx::Handle<Commodity> m_commodity;
    std::set<wxString> m_account_names_already_taken;

    wxStaticText* m_summary_amount_text;
    
    size_t m_minimum_num_rows;

    std::vector<TextCtrl*> m_account_name_boxes;
    std::vector<AccountTypeCtrl*> m_account_type_boxes;
    std::vector<TextCtrl*> m_description_boxes;
    std::vector<OpeningBalanceCtrl*> m_opening_balance_boxes;
    std::vector<CheckBox*> m_check_boxes;

};  // class MultiAccountPanel


// IMPLEMENT MEMBER TEMPLATES AND INLINE FUNCTIONS

template <typename T>
void
MultiAccountPanel::remove_widgets_from(std::vector<T>& p_vec)
{
    for (auto* elem: p_vec)
    {
        top_sizer().Detach(elem);
        elem->Destroy();
        elem = nullptr;
    }
    p_vec.clear();
    return;
}

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_multi_account_panel_hpp_2678521416465995

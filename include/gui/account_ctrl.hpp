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

#ifndef GUARD_account_ctrl_hpp_7150714070140717
#define GUARD_account_ctrl_hpp_7150714070140717

#include "account.hpp"
#include "account_type.hpp"
#include "gui/combo_box.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/window.h>
#include <map>
#include <set>
#include <vector>

namespace dcm
{

// begin forward declarations

class DcmDatabaseConnection;

namespace gui
{

// end forward declarations

/**
 * Widget by means of which the user is enabled to select an
 * \e existing Account. Normally only Accounts of Visibility::visible
 * are shown; however, calling set_account(...) will cause the selected
 * Account to be shown even if it is of Visibility::hidden.
 */
class AccountCtrl: public ComboBox
{
public:

    /**
     * @param p_parent parent window.
     *
     * @param p_id id.
     *
     * @param p_size size.
     *
     * @param p_account_types the AccountTypes we want represented in the
     * AccountCtrl. This must not be empty.
     *
     * @param p_database_connection connection to database where Account
     * details are persisted.
     *
     * @param p_exclude_balancing_account If \e true, then the
     * sqloxx::Handle<Account> returned by
     * p_account.database_connection().balancing_account() will not
     * appear in the Combobox, even if it is in the range provided.
     *
     * @throws InvalidAccountTypeException if p_account_types is empty.
     *
     * @throws InvalidAccountException if p_account_types is non-empty, but
     * there are no Accounts with these AccountTypes in the database.
     */
    AccountCtrl
    (   wxWindow* p_parent,
        unsigned int p_id,
        wxSize const& p_size,
        std::vector<AccountType> const& p_account_types,
        DcmDatabaseConnection& p_database_connection,
        bool p_exclude_balancing_account = false
    );

    AccountCtrl(AccountCtrl const&) = delete;
    AccountCtrl(AccountCtrl&&) = delete;
    AccountCtrl& operator=(AccountCtrl const&) = delete;
    AccountCtrl& operator=(AccountCtrl&&) = delete;
    virtual ~AccountCtrl();

    /**
     * Reset the selections available in the Combobox, to all the
     * Accounts with AccountTypes given by p_account_types.
     * However, if p_exclude_balancing_account is \e true, then
     * the sqloxx::Handle<Account> returned by
     * <em>m_database_connection.balancing_account()</em> will \e not
     * appear in the Combobox, even if its AccountType is in the range provided.
     *
     * @throws InvalidAccountTypeException if p_account_types is empty.
     *
     * @throws InvalidAccountException if p_account_types is non-empty, but
     * there are no Accounts with these AccountTypes in the database.
     */
    void reset_for_account_types
    (   std::vector<AccountType> const& p_account_types,
        bool p_exclude_balancing_account = false
    );

    /**
     * @returns sqloxx::Handle to currently selected Account.
     *
     * Unfortunately this can't currently be const because the
     * wxWindow::GetValidator() function used in the implementation, is
     * not const.
     */
    sqloxx::Handle<Account> account();

    /**
     * Sets displayed Account to p_account. If p_account is not already
     * available within the AccountCtrl, then this will add it to the
     * AccountCtrl (even if it is of Visibility::hidden). \e But note
     * the exceptions below.
     *
     * @throws InvalidAccountException if p_account is not of an
     * AccountType that is supported by this AccountCtrl, or if
     * p_preserved_account is the balancing Account and
     * p_exclude_balancing_account was set to \e true.
     */
    void set_account(sqloxx::Handle<Account> const& p_account);

    void update_for_new(sqloxx::Handle<Account> const& p_account);
    void update_for_amended(sqloxx::Handle<Account> const& p_account);


private:

    void on_kill_focus(wxFocusEvent& event);

    /**
     * @returns \e true if and only if p_account_type is supported by the
     * AccountCtrl.
     */
    bool supports_account_type(AccountType p_account_type);

    /**
     * Reset the available Accounts in the AccountCtrl, by rereading the Account
     * details from the database. If \e p_preserve_account is non-null, always
     * include and display this Account, even if it is of Visibility::hidden
     * (but see exceptions below).
     *
     * However, if p_preserved_account is the balancing Account, and
     * p_exclude_balancing_account was set to \e true, then it will always
     * be excluded regardless.
     *
     * @throws InvalidAccountException if p_preserve_account is not of an
     * AccountType that is supported by this AccountCtrl, or if
     * p_preserved_account is the balancing Account and
     * p_exclude_balancing_account was set to \e true.
     */
    void reset
    (   sqloxx::Handle<Account> const& p_preserve_account =
            sqloxx::Handle<Account>()
    );

    void refresh();
    bool m_exclude_balancing_account;
    DcmDatabaseConnection& m_database_connection;
    typedef std::map<wxString, sqloxx::Id> AccountMap;
    AccountMap m_account_map;
    std::set<AccountType> m_available_account_types;
    DECLARE_EVENT_TABLE()

};  // class AccountCtrl


}  // namespace gui
}  // namespace dcm

#endif  // GUARD_account_ctrl_hpp_7150714070140717

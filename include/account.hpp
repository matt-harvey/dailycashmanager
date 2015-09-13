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

#ifndef GUARD_account_hpp_5971945187063862
#define GUARD_account_hpp_5971945187063862

#include "account_type_fwd.hpp"
#include "date.hpp"
#include "finformat.hpp"
#include "dcm_database_connection.hpp"
#include "string_conv.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <boost/optional.hpp>
#include <wx/string.h>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dcm
{

// begin forward declarations

class BudgetItem;
class Commodity;

// end forward declarations

/**
 * Represents an account - which could be either a balance sheet account,
 * or an P&L account. The latter is equivalent to an "envelope" or
 * "category".
 * 
 * Client code must deal with sqloxx::Handle<Account>, rather than with
 * Account objects directly. This is enforced via "Signature" parameters
 * in the constructors for Account.
 */
class Account:
    public sqloxx::PersistentObject<Account, DcmDatabaseConnection>
{
public:
    
    typedef sqloxx::PersistentObject<Account, DcmDatabaseConnection>
        PersistentObject;

    typedef sqloxx::IdentityMap<Account> IdentityMap;

    /**
     * Sets up tables in the database required for the persistence of
     * Account objects.
     */
    static void setup_tables(DcmDatabaseConnection& dbc);

    /**
     * @returns the Id of the Account with name p_name, matched
     * case insensitively. If
     * there are multiple such Accounts, then it is undefined
     * which Id will be returned, but one will be.
     *
     * @throws InvalidAccountNameException if there is no Account
     * named p_name (this is tested case insensitively).
     */
    static sqloxx::Id id_for_name
    (   DcmDatabaseConnection& dbc,
        wxString const& p_name
    );

    /**
     * Initialize a "draft" Account, that will not correspond to any
     * particular object in the database.
     *
     * The Signature parameter ensures that this can only be called by
     * IdentityMap. Ordinary client code should use sqloxx::Handle<Account>,
     * not Account.
     */
    Account
    (   IdentityMap& p_identity_map,
        IdentityMap::Signature const& p_signature
    );

    /**
     * Get an Account by id from database.
     *
     * The Signature parameter esures that this can only be called
     * by IdentityMap. Ordinary client code should use
     * sqloxx::Handle<Account>, not Account.
     */
    Account
    (   IdentityMap& p_identity_map,
        sqloxx::Id p_id,
        IdentityMap::Signature const& p_signature
    );

    /**
     * @returns \e true if and only if \e p_name is the name of an Account
     * stored in the database, matched case insensitively.
     *
     * @param p_database_connection connection to database
     *
     * @param p_name name of Account
     */
    static bool exists
    (   DcmDatabaseConnection& p_database_connection,
        wxString const& p_name
    );

    /**
     * Like PersistentObject::exists. Declaring as direct member of Account
     * so as to avoid overload issues with "exists" function that takes
     * wxString.
     */
    static bool exists
    (   DcmDatabaseConnection& p_database_connection,
        sqloxx::Id p_id
    );

    /**
     * @returns \e true if the only P&L Account that is saved is
     * p_database_connection.balancing_account(), or if there are no
     * P&L Accounts saved at all. If there is one or more P&L Account saved
     * other than p_database_connection.balancing_account(), then
     * this function returns \e false.
     */
    static bool no_user_pl_accounts_saved
    (   DcmDatabaseConnection& p_database_connection
    );

    // copy constructor is private

    Account(Account&&) = delete;
    Account& operator=(Account const&) = delete;
    Account& operator=(Account&&) = delete;
    ~Account();

    /**
     * @returns name of Account.
     */
    wxString name();

    /**
     * @returns the native Commodity of the Account.
     */
    sqloxx::Handle<Commodity> commodity();

    /**
     * @returns the AccountType of the Account.
     */
    AccountType account_type();

    /**
     * @returns the AccountSuperType of the Account.
     */
    AccountSuperType account_super_type();

    /**
     * @returns the Account description.
     */
    wxString description();

    /**
     * @returns the Visibility of the Account.
     */
    Visibility visibility();

    /**
     * @returns "technical" account balance, which is
     * positive for debit balances and negative for
     * credit balances. For P&L accounts this
     * corresponds to the unspent funds in the envelope,
     * where a negative balance indicates that funds
     * remain unspent, and a positive balance indicates
     * that more funds have been spend than were
     * available.
     */
    jewel::Decimal technical_balance();

    /**
     * @returns "user friendly" account balance, which, for
     * P&L accounts only, has the signs reversed relative to
     * technical_balance().
     */
    jewel::Decimal friendly_balance();

    /**
     * @returns "technical" opening balance of Account.
     * See documentation for technical_balance() and
     * friendly_balance(), for explanation
     * of difference between "technical" and "friendly"
     * balance.
     */
    jewel::Decimal technical_opening_balance();

    /**
     * @returns "user-friendly" opening balance of Account,
     * See documentation for technical_balance() and
     * friendly_balance(), for explanation
     * of difference between "technical" and "friendly"
     * balance.
     */
    jewel::Decimal friendly_opening_balance();

    /**
     * @returns the amount of the recurring budget for
     * the account, in terms of the standard
     * Frequency for database_connection().
     * E.g. if the standard Frequency is daily, then
     * then a budget() of 100.00 implies 100.00/day.
     */
    jewel::Decimal budget();

    /**
     * @returns a std::vector containing all and only
     * the saved BudgetItems for this Account, sorted in ascending order
     * by the Id of each BudgetItem.
     */
    std::vector<sqloxx::Handle<BudgetItem> > budget_items();

    /**
     * Set the AccountType to \e p_account_type.
     */
    void set_account_type(AccountType p_account_type);

    /**
     * Set the name of the Account to \e p_name.
     */
    void set_name(wxString const& p_name);

    /**
     * Set the native Commodity of the Account to \e p_commodity.
     */
    void set_commodity(sqloxx::Handle<Commodity> const& p_commodity);

    /**
     * Set the description of the Account to \e p_description.
     */
    void set_description(wxString const& p_description);

    /**
     * Set the Visibility of the Account to \e p_visibility.
     */
    void set_visibility(Visibility p_visibility);

    /**
     * @returns the name of the "exclusive" database table for the Account
     * class, i.e. the table holding all and only the primary keys for saved
     * Account instances.
     *
     * @note This is tied to the Sqloxx API, which requires std::string here,
     * not wxString.
     */
    static std::string exclusive_table_name();

    /**
     * @returns the name of the primary key for Account instances as stored in
     * the database.
     *
     * @note This is tied to the Sqloxx API, which requires std::string here,
     * not wxString.
     */
    static std::string primary_key_name();

private:

    void swap(Account& rhs);

    /**
     * Copy constructor - implemented, but deliberately private.
     */
    Account(Account const& rhs);

    void do_load() override;
    void do_save_existing() override;
    void do_save_new() override;
    void do_ghostify() override;
    void do_remove() override;
    void process_saving_statement(sqloxx::SQLStatement& statement);

    struct AccountData;

    std::unique_ptr<AccountData> m_data;
};


// NON-MEMBER FUNCTIONS

/**
 * Type used to pass options to \e account_concept_name and
 * \e account_concepts_phrase functions. For interface, see
 * documentation for \e jewel::FlagSet, in the Jewel library.
 */
typedef
    jewel::FlagSet
    <    string_flags::StringFlags,
        string_flags::capitalize |
        string_flags::include_article |
        string_flags::pluralize
    >
    AccountPhraseFlags;

/**
 * @returns "account", "category" or some such string to describe
 * to the \e user the "thing" which they are creating in this
 * particular MultiAccountPanel.
 */
wxString account_concept_name
(   AccountSuperType p_account_super_type,
    AccountPhraseFlags p_phrase_flag_set = AccountPhraseFlags()
);

/**
 * @returns a string of the form "X or Y", where "X" and "Y" are the names
 * of the different possible "account concepts" (e.g. this might
 * return something like "account or category").
 */
wxString account_concepts_phrase
(   AccountPhraseFlags p_phrase_flag_set = AccountPhraseFlags()
);

/**
 * @returns a map which indicates, for each AccountSuperType,
 * the Account of that AccountSuperType which has the largest number of
 * ActualOrdinaryEntries (however, the budget balancing Account is
 * never included, nor are any pure_envelope Accounts).
 */
std::map<AccountSuperType, sqloxx::Id>
favourite_accounts(DcmDatabaseConnection& p_database_connection);

}  // namespace dcm

#endif  // GUARD_account_hpp_5971945187063862

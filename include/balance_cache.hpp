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

#ifndef GUARD_balance_cache_hpp_3730216051326234
#define GUARD_balance_cache_hpp_3730216051326234

#include "account.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace dcm
{

// begin forward declarations

class DcmDatabaseConnection;

// end forward declarations


// Class-by-class summary of what triggers staleness:
// Account - any database-affecting operations on any
// Account should make the whole map
// stale (to keep things simple, as in any case Account operations would
// be relatively rare).
// Commodity - any database-affecting operations on any Commodity
// should make the whole
// map stale (to keep things simple, as in any case Commodity operations
// would be relatively rare).
// Entry - whenever an Entry is operated on that has a particular
// Account as its Account, the cache entry for that Account should be
// marked as stale (if the operation affects the database).
// Journal, DraftJournal and OrdinaryJournal - Any operations on these
// that affect Account balances will do so only insofar as they involve
// operations on Entries. Therefore, Draft/Ordinary/Journal operations
// as such do not need to trigger BalanceCache staleness.
// Repeater. These contain only draft Entries so do not need to trigger
// staleness.


/**
 * Provides a cache for holding Account balances.
 */
class BalanceCache
{
public:

    static void setup_tables(DcmDatabaseConnection& dbc);

    BalanceCache(DcmDatabaseConnection& p_database_connection);

    BalanceCache(BalanceCache const&) = delete;
    BalanceCache(BalanceCache&&) = delete;
    BalanceCache& operator=(BalanceCache const&) = delete;
    BalanceCache& operator=(BalanceCache&&) = delete;

    ~BalanceCache();

    /**
     * Retrieve the technical balance for a particular Account.
     * For an explanation of the concept of a "technical balance",
     * see the documentation for Account::technical_balance().
     */
    jewel::Decimal technical_balance(sqloxx::Id p_account_id);

    /**
     * Retrieve the technical opening balance for a particular
     * Account.
     * For an explanation of the concept of a "technical opening balance",
     * see the documentation for Account::technical_opening_balance().
     */
    jewel::Decimal technical_opening_balance(sqloxx::Id p_account_id);

    /**
     * Mark the cache as a whole as stale.
     */
    void mark_as_stale();
    
    /**
     * Mark a particular Account's cache entry as stale.
     */
    void mark_as_stale(sqloxx::Id p_account_id); 

private:

    typedef
        std::unordered_map
        <    sqloxx::Id,
            boost::optional<jewel::Decimal>
        >
        Map;
        
    void refresh();
    void refresh_all();
    void refresh_targetted(std::vector<sqloxx::Id> const& p_targets);

    DcmDatabaseConnection& m_database_connection;
    std::unique_ptr<Map> m_map;
    bool m_map_is_stale;

};



}  // namespace dcm


#endif  // GUARD_balance_cache_hpp_3730216051326234

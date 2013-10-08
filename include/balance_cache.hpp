// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_balance_cache_hpp_3730216051326234
#define GUARD_balance_cache_hpp_3730216051326234

#include "account.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <memory>
#include <unordered_map>
#include <vector>


namespace phatbooks
{

class PhatbooksDatabaseConnection;
// End forward declarations

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
 *
 * @todo Testing.
 */
class BalanceCache
{
public:

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	BalanceCache(PhatbooksDatabaseConnection& p_database_connection);

	BalanceCache(BalanceCache const&) = delete;
	BalanceCache(BalanceCache&&) = delete;
	BalanceCache& operator=(BalanceCache const&) = delete;
	BalanceCache& operator=(BalanceCache&&) = delete;

	~BalanceCache() = default;

	// Retrieve the technical balance for a particular Account.
	// For an explanation of the concept of a "technical balance",
	// see the documentation for Account::technical_balance().
	jewel::Decimal technical_balance(sqloxx::Id p_account_id);

	// Retrieve the technical opening balance for a particular
	// Account.
	// For an explanation of the concept of a "technical opening balance",
	// see the documentation for Account::technical_opening_balance().
	jewel::Decimal technical_opening_balance(sqloxx::Id p_account_id);

	// Mark the cache as a whole as stale
	void mark_as_stale();
	
	// Mark a particular Account's cache entry as stale
	void mark_as_stale(sqloxx::Id p_account_id); 

private:

	typedef
		std::unordered_map
		<	sqloxx::Id,
			boost::optional<jewel::Decimal>
		>
		Map;
		
	void refresh();
	void refresh_all();
	void refresh_targetted(std::vector<sqloxx::Id> const& p_targets);

	PhatbooksDatabaseConnection& m_database_connection;
	std::unique_ptr<Map> m_map;
	bool m_map_is_stale;

};



}  // namespace phatbooks


#endif  // GUARD_balance_cache_hpp_3730216051326234

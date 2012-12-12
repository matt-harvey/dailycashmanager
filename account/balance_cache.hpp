#ifndef GUARD_balance_cache_hpp
#define GUARD_balance_cache_hpp

#include "account.hpp"
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>

// Forward declarations
namespace jewel
{
	class Decimal;
}

namespace phatbooks
{

class Account;
class PhatbooksDatabaseConnection;
// End forward declarations

// WARNING
// Class-by-class summary of what triggers staleness:
// Account - any operations on any Account should make the whole map
// stale (to keep things simple, as in any case Account operations would
// be relatively rare).
// Commodity - any operations on any Commodity should make the whole
// map stale (to keep things simple, as in any case Commodity operations
// would be relatively rare).
// Entry - whenever an Entry is operated on that has a particular Account
// as its Account, the cache entry for that Account should be marked as
// stale.
// Journal, DraftJournal and OrdinaryJournal - Any operations on these
// that affect Account balances will do so only insofar as they involve
// operations on Entries. Therefore, Draft/Ordinary/Journal operations
// as such do not need to trigger BalanceCache staleness.
// Repeater. These contain only draft Entries so do not need to trigger
// staleness.


/**
 * Provides a cache for holding Account balances.
 */
class BalanceCache:
	public boost::noncopyable
{
public:

	BalanceCache(PhatbooksDatabaseConnection& p_database_connection);

	// Retrieve the balance for a particular Account. The cache will
	// refresh itself if required.
	jewel::Decimal balance(Account const& p_account);

	// Mark the cache as a whole as stale
	void mark_as_stale();
	
	// Mark a particular Account's cache entry as stale
	void mark_as_stale(Account const& p_account); 

private:

	typedef
		boost::unordered_map
		<	Account::Id,
			boost::optional<jewel::Decimal>
		>
		Map;
		
	void refresh();

	PhatbooksDatabaseConnection& m_database_connection;
	boost::scoped_ptr<Map> m_map;
	bool m_map_is_stale;

};



}  // namespace phatbooks


#endif  // GUARD_balance_cache_hpp

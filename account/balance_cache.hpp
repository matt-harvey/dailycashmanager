#ifndef GUARD_balance_cache_hpp
#define GUARD_balance_cache_hpp

#include "account_impl.hpp"
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

class PhatbooksDatabaseConnection;
// End forward declarations

// WARNING
// Class-by-class summary of what triggers staleness:
// AccountImpl - any database-affecting operations on any
// AccountImpl should make the whole map
// stale (to keep things simple, as in any case AccountImpl operations would
// be relatively rare).
// CommodityImpl - any database-affecting operations on any CommodityImpl
// should make the whole
// map stale (to keep things simple, as in any case CommodityImpl operations
// would be relatively rare).
// EntryImpl - whenever an EntryImpl is operated on that has a particular
// AccountImpl
// as its AccountImpl, the cache entry for that AccountImpl should be
// marked as stale (if the operation affects the database).
// Journal, DraftJournal and OrdinaryJournal - Any operations on these
// that affect AccountImpl balances will do so only insofar as they involve
// operations on Entries. Therefore, Draft/Ordinary/Journal operations
// as such do not need to trigger BalanceCache staleness.
// Repeater. These contain only draft Entries so do not need to trigger
// staleness.


/**
 * Provides a cache for holding AccountImpl balances.
 */
class BalanceCache:
	public boost::noncopyable
{
public:

	BalanceCache(PhatbooksDatabaseConnection& p_database_connection);

	// Retrieve the balance for a particular AccountImpl. The cache will
	// refresh itself if required.
	jewel::Decimal balance(AccountImpl::Id p_account_id);

	// Mark the cache as a whole as stale
	void mark_as_stale();
	
	// Mark a particular AccountImpl's cache entry as stale
	void mark_as_stale(AccountImpl::Id p_account_id); 

private:

	typedef
		boost::unordered_map
		<	AccountImpl::Id,
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

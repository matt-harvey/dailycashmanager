#include "account.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "entry.hpp"
#include "balance_cache.hpp"
#include "phatbooks_database_connection.hpp"
#include "entry/entry_reader.hpp"
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>

using boost::optional;
using boost::scoped_ptr;
using jewel::Decimal;
using jewel::clear;
using jewel::value;

// For debugging only
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff

namespace phatbooks
{


BalanceCache::BalanceCache
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection),
	m_map(new Map),
	m_map_is_stale(true)
{
}


Decimal
BalanceCache::technical_balance(AccountImpl::Id p_account_id)
{
	AccountImpl::Id const id = p_account_id;
	if (m_map_is_stale)
	{
		refresh();
	}
	else
	{
		Map::iterator const it = m_map->find(id);

		// If a new AccountImpl has been added, then the AccountImpl
		// class should have marked the map as a whole as stale,
		// and the earlier call to refresh() should have
		// inserted a cache entry for that AccountImpl. Thus at this
		// point there must be a cache entry for p_account_id.
		assert (it != m_map->end());
		optional<Decimal> const cache_entry(it->second);	
		if (cache_entry)
		{
			return *cache_entry;
		}
		else
		{
			// cache entry is stale	
			refresh();
		}
	}
	return *(m_map->find(id)->second);
}


void
BalanceCache::mark_as_stale()
{
	m_map_is_stale = true;
}


void
BalanceCache::mark_as_stale(AccountImpl::Id p_account_id)
{
	Map::iterator const it = m_map->find(p_account_id);
	if (it == m_map->end())
	{
		m_map_is_stale = true;
		return;
	}
	assert (it != m_map->end());
	clear(it->second);
	return;
}


void
BalanceCache::refresh()
{
	// Note, if I end up turning off the sqloxx::IdentityMap's
	// caching (which is totally separate to this balance caching), I may wish
	// to speed up the below by using SQL sum function etc.. 
	// This would be very fast, and avoid having to load Entry
	// objects. However, with IdentityMap's caching enabled, it is
	// simpler and more maintainable for the below to be implemented
	// as shown here.
	JEWEL_DEBUG_LOG << "Refreshing balance cache..." << endl;
	scoped_ptr<Map> map_elect_ptr(new Map);	
	Map& map_elect = *map_elect_ptr;
	AccountReader account_reader(m_database_connection);
	for
	(	AccountReader::const_iterator it = account_reader.begin(),
			end = account_reader.end();
		it != end;
		++it
	)
	{
		Account const account(*it);
		map_elect[account.id()] =
			Decimal(0, account.commodity().precision());
	}
	OrdinaryEntryReader entry_reader(m_database_connection);
	for
	(	OrdinaryEntryReader::const_iterator it = entry_reader.begin(),
			end = entry_reader.end();
		it != end;
		++it
	)
	{
		Entry const entry(*it);
		*(map_elect[entry.account().id()]) += entry.amount();
	}
	using std::swap;
	swap(m_map, map_elect_ptr);
	m_map_is_stale = false;
	return;
}



}  // namespace phatbooks

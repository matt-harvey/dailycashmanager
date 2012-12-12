#include "account.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "entry.hpp"
#include "balance_cache.hpp"
#include "phatbooks_database_connection.hpp"
#include "entry/entry_reader.hpp"
#include <boost/scoped_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>

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
BalanceCache::balance(AccountImpl::Id p_account_id)
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

		if ( !(it->second) )
		{
			// Cache entry for p_account_id is stale
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
	JEWEL_DEBUG_LOG << "Refreshing balance cache..." << endl;
	scoped_ptr<Map> map_elect_ptr(new Map);	
	Map& map_elect = *map_elect_ptr;
	AccountReader account_reader(m_database_connection);
	while (account_reader.read())
	{
		Account const account(account_reader);
		map_elect[account.id()] =
			Decimal(0, account.commodity().precision());
	}
	OrdinaryEntryReader entry_reader(m_database_connection);
	while (entry_reader.read())
	{
		Entry const entry(entry_reader);
		*(map_elect[entry.account().id()]) += entry.amount();
	}
	using std::swap;
	swap(m_map, map_elect_ptr);
	m_map_is_stale = false;
	return;
}



}  // namespace phatbooks

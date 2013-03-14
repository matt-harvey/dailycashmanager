#include "account.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "entry.hpp"
#include "balance_cache.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "entry/entry_reader.hpp"
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/checked_arithmetic.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/sql_statement.hpp>
#include <algorithm>
#include <cassert>

using boost::optional;
using boost::scoped_ptr;
using boost::unordered_map;
using jewel::addition_is_unsafe;
using jewel::Decimal;
using jewel::clear;
using jewel::value;
using sqloxx::SQLStatement;

// For debugging only
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff

namespace phatbooks
{

void
BalanceCache::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create index entry_account_index on entries(account_id)"
	);
	return;
}

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
	// TODO We need to mark the BalanceCache as stale whenever there is a
	// a change in date; this might occur while the user is using the
	// application. We can do this as follows. We store the date
	// when the balance cache was last updated. Whenever there is
	// a request for a balance, we compare the last update date
	// with today's date. If the latter is later than the former,
	// then we mark as stale and do an update. If this occurs, then
	// we should also check the Repeaters for recurring DraftJournal
	// autoposts while we're at it. We should notify the user of any
	// automatically posted transactions in a way that doesn't interfere
	// with their current "workflow".
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
	// TODO Sort out whether, conceptually, we are looking at
	// Account::Id, or at AccountImpl::Id. Make sure the header reflects
	// this too. (In practice it doesn't matter though.)
	typedef unordered_map<Account::Id, Decimal::int_type> WorkingMap;
	WorkingMap working_map;
	AccountReader account_reader(m_database_connection);
	assert (working_map.empty());
	for
	(	AccountReader::const_iterator it = account_reader.begin(),
			end = account_reader.end();
		it != end;
		++it
	)
	{
		working_map[it->id()] = 0;
	}
	
	// It has been established that this is faster than using SQL
	// SUM and GROUP to sum Account totals. Note also that we have
	// chosen not to use an OrdinaryEntryReader here: we don't want
	// to load all the non-actual Entries into memory.

	// Standalone scope
	{
		SQLStatement statement
		(	m_database_connection,
			"select account_id, amount from entries join "
			"ordinary_journal_detail using(journal_id)"
		);
		Account::Id account_id;
		Decimal::int_type amount_intval;
		while (statement.step())
		{
			account_id = statement.extract<Account::Id>(0);
			amount_intval = statement.extract<Decimal::int_type>(1);
			if (addition_is_unsafe(working_map[account_id], amount_intval))
			{
				throw UnsafeArithmeticException
				(	"Unsafe addition while refreshing BalanceCache."
				);
			}
			working_map[account_id] += amount_intval;
		}
	}
	scoped_ptr<Map> map_elect_ptr(new Map);	
	Map& map_elect = *map_elect_ptr;
	assert (map_elect.empty());
	for
	(	WorkingMap::const_iterator it = working_map.begin(),
			end = working_map.end();
		it != end;
		++it
	)
	{
		Account::Id const account_id = it->first;
		Account const account(m_database_connection, account_id);
		map_elect[account_id] =
			Decimal(it->second, account.commodity().precision());
	}
	assert (map_elect.size() == working_map.size());
	assert (map_elect_ptr->size() == map_elect.size());
	using std::swap;
	swap(m_map, map_elect_ptr);
	m_map_is_stale = false;
	return;
}



}  // namespace phatbooks

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
#include <vector>

using boost::optional;
using boost::scoped_ptr;
using boost::unordered_map;
using jewel::addition_is_unsafe;
using jewel::Decimal;
using jewel::clear;
using jewel::value;
using sqloxx::SQLStatement;
using std::vector;

// For debugging only
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff

// TODO Sort out whether, conceptually, we are looking at
// Account::Id, or at AccountImpl::Id. Amend code here to reflect
// (currently it is confused in regards to this). Make sure the header
// reflects this too. (In practice it doesn't matter though.)

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
	JEWEL_DEBUG_LOG << "Refreshing balance cache" << endl;


	// Here we decide whether it's quickest to do a complete rebuild of
	// the entire cache, or whether it's quicker just the
	// stale accounts. (Either way, we end
	// up with totally refreshed cache at the end - this is
	// purely an optimization decision.)

	// TODO Figure out the best value for fulcrum.

	static AccountReader::size_type const fulcrum = 5;
	AccountReader account_reader(m_database_connection);
	vector<AccountImpl::Id> stale_account_ids;
	SQLStatement statement
	(	m_database_connection,
		"select account_id from accounts"
	);
	Map::const_iterator const map_end = m_map->end();
	while (statement.step() && stale_account_ids.size() != fulcrum)
	{
		// TODO HIGH PRIORITY. What if an Account has been
		// removed from the database? This loop needs won't find these!
		// They will stay in the cache.
		AccountImpl::Id const account_id =
			statement.extract<AccountImpl::Id>(0);
		Map::const_iterator location_in_cache = m_map->find(account_id);
		if (location_in_cache == map_end || !(location_in_cache->second))
		{
			// Either this Account::Id is not in the cache at all,
			// or it's in there but marked as stale.
			stale_account_ids.push_back(account_id);
		}
	}
	assert (stale_account_ids.size() <= fulcrum);
	if (stale_account_ids.size() == fulcrum)
	{
		JEWEL_DEBUG_LOG << "Calling BalanceCache::refresh_all()" << endl;
		refresh_all();
	}
	else
	{
		assert (stale_account_ids.size() < fulcrum);
		JEWEL_DEBUG_LOG << "Calling BalanceCache::refresh_targetted() "
		                << "with " << stale_account_ids.size()
						<< " stale accounts_ids"
						<< endl;
		refresh_targetted(stale_account_ids);
	}
	m_map_is_stale = false;
	return;
}

void
BalanceCache::refresh_all()
{
	typedef unordered_map<Account::Id, Decimal::int_type> WorkingMap;
	WorkingMap working_map;
	AccountReader account_reader(m_database_connection);
	assert (working_map.empty());
	SQLStatement accounts_scanner
	(	m_database_connection,
		"select account_id from accounts"
	);
	while (accounts_scanner.step())
	{
		working_map[accounts_scanner.extract<Account::Id>(0)] = 0;
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
	return;
}


void
BalanceCache::refresh_targetted(vector<AccountImpl::Id> const& p_targets)
{
	// TODO Is this exception safe?
	typedef vector<AccountImpl::Id> IdVec;
	for
	(	IdVec::const_iterator it = p_targets.begin(), end = p_targets.end();
		it != end;
		++it
	)
	{
		AccountImpl::Id const account_id = *it;
		Account const account(m_database_connection, account_id);
		SQLStatement statement
		(	m_database_connection,
			"select sum(amount) from entries join ordinary_journal_detail "
			"using(journal_id) where account_id = :account_id"
		);
		statement.bind(":account_id", account_id);
		if (statement.step())
		{
			(*m_map)[account_id] = Decimal
			(	statement.extract<Decimal::int_type>(0),
				account.commodity().precision()
			);
			statement.step_final();
		}
		else
		{
			(*m_map)[account_id] =
				Decimal(0, account.commodity().precision());
		}
	}
	return;
}

}  // namespace phatbooks

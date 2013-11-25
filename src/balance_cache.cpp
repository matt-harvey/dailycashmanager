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

#include "account.hpp"
#include "date.hpp"
#include "balance_cache.hpp"
#include "commodity.hpp"
#include "dcm_database_connection.hpp"
#include "dcm_exceptions.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/checked_arithmetic.hpp>
#include <jewel/decimal.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <sqloxx/sql_statement.hpp>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

using boost::optional;
using jewel::addition_is_unsafe;
using jewel::Decimal;
using jewel::clear;
using jewel::value;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::SQLStatement;
using sqloxx::ValueTypeException;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

namespace dcm
{

void
BalanceCache::setup_tables(DcmDatabaseConnection& dbc)
{
	JEWEL_LOG_TRACE();
	dbc.execute_sql
	(	"create index entry_account_index on entries(account_id)"
	);
	JEWEL_LOG_TRACE();
	return;
}

BalanceCache::BalanceCache
(	DcmDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection),
	m_map(new Map),
	m_map_is_stale(true)
{
	JEWEL_LOG_TRACE();
}

BalanceCache::~BalanceCache()
{
	JEWEL_LOG_TRACE();
}

Decimal
BalanceCache::technical_balance(sqloxx::Id p_account_id)
{
	if (m_map_is_stale)
	{
		refresh();
	}
	else
	{
		Map::iterator const it = m_map->find(p_account_id);

		// If a new Account has been added, then the Account
		// class should have marked the map as a whole as stale,
		// and the earlier call to refresh() should have
		// inserted a cache entry for that Account. Thus at this
		// point there must be a cache entry for p_account_id.
		JEWEL_ASSERT (it != m_map->end());
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
	return *(m_map->find(p_account_id)->second);
}

Decimal
BalanceCache::technical_opening_balance(sqloxx::Id p_account_id)
{
	// We don't actually do any caching of opening balances, since
	// they are quick to calculate. (We would expect only a small number
	// of opening balance journals for any given Account.)
	SQLStatement statement
	(	m_database_connection,
		"select sum(amount) from ordinary_journal_detail "
		"join entries using(journal_id) where date = :date "
		"and account_id = :account_id"
	);
	statement.bind
	(	":date",
		julian_int(m_database_connection.opening_balance_journal_date())
	);
	statement.bind(":account_id", p_account_id);
	Handle<Account> const account(m_database_connection, p_account_id);
	Decimal::places_type const places = account->commodity()->precision();
	Decimal ret(0, places);
	if (statement.step())
	{
		try
		{
			ret = Decimal(statement.extract<Decimal::int_type>(0), places);
		}
		catch (ValueTypeException&)
		{
			// There are no entries to sum - leave ret as zero
		}
		statement.step_final();
	}
	else
	{
		// There should be a result row even if there are no entries
		// to sum
		JEWEL_HARD_ASSERT (false);
	}
	return ret;
}

void
BalanceCache::mark_as_stale()
{
	m_map_is_stale = true;
}

void
BalanceCache::mark_as_stale(sqloxx::Id p_account_id)
{
	Map::iterator const it = m_map->find(p_account_id);
	if (it == m_map->end())
	{
		m_map_is_stale = true;
		return;
	}
	JEWEL_ASSERT (it != m_map->end());
	clear(it->second);
	return;
}

void
BalanceCache::refresh()
{
	// Here we decide whether it's quicker to do a complete rebuild of
	// the entire cache, or whether it's quicker just to update for the
	// stale accounts. (Either way, we end
	// up with totally refreshed cache at the end - this is
	// purely an optimization decision.) The exact size of the optimal
	// fulcrum here is an educated guess made after some casual
	// experimentation. There is scope for further optimization if
	// required, by tweaking this fulcum figure.
	
	static vector<sqloxx::Id>::size_type const fulcrum = 5;
	
	if (m_map_is_stale)
	{
		refresh_all();
		m_map_is_stale = false;
	}
	else
	{
		vector<sqloxx::Id> stale_account_ids;
		SQLStatement statement
		(	m_database_connection,
			"select account_id from accounts"
		);
		Map::const_iterator const map_end = m_map->end();
		while (statement.step() && (stale_account_ids.size() != fulcrum))
		{
			sqloxx::Id const account_id =
				statement.extract<sqloxx::Id>(0);
			Map::const_iterator location_in_cache = m_map->find(account_id);
			if ((location_in_cache == map_end) || !location_in_cache->second)
			{
				// Either this sqloxx::Id is not in the cache at all,
				// or it's in there but marked as stale.
				stale_account_ids.push_back(account_id);
			}
		}
		JEWEL_ASSERT (stale_account_ids.size() <= fulcrum);
		if (stale_account_ids.size() == fulcrum)
		{
			refresh_all();
		}
		else
		{
			JEWEL_ASSERT (stale_account_ids.size() < fulcrum);
			refresh_targetted(stale_account_ids);
		}
	}
	JEWEL_ASSERT (!m_map_is_stale);
	return;
}

void
BalanceCache::refresh_all()
{
	typedef unordered_map<sqloxx::Id, Decimal::int_type> WorkingMap;
	WorkingMap working_map;
	JEWEL_ASSERT (working_map.empty());
	SQLStatement accounts_scanner
	(	m_database_connection,
		"select account_id from accounts"
	);
	while (accounts_scanner.step())
	{
		working_map[accounts_scanner.extract<sqloxx::Id>(0)] = 0;
	}
	
	// It has been established that this is faster than using SQL
	// SUM and GROUP to sum Account totals.

	// Bare scope
	{
		// Ordering by entry_id to decrease the likelihood of
		// "intermediate overflow". Also, consider the effect on the integrity
		// of PersistentJournal::would_cause_overflow().
		SQLStatement statement
		(	m_database_connection,
			"select account_id, amount from entries join "
			"ordinary_journal_detail using(journal_id) order by entry_id"
		);
		sqloxx::Id account_id;
		Decimal::int_type amount_intval;
		while (statement.step())
		{
			account_id = statement.extract<sqloxx::Id>(0);
			amount_intval = statement.extract<Decimal::int_type>(1);
			if (addition_is_unsafe(working_map[account_id], amount_intval))
			{
				JEWEL_THROW
				(	UnsafeArithmeticException,
					"Unsafe addition while refreshing BalanceCache."
				);
			}
			working_map[account_id] += amount_intval;
		}
	}
	unique_ptr<Map> map_elect_ptr(new Map);	
	Map& map_elect = *map_elect_ptr;
	JEWEL_ASSERT (map_elect.empty());

	for (auto const& working_map_elem: working_map)
	{
		Id const account_id = working_map_elem.first;
		Handle<Account> const account(m_database_connection, account_id);
		map_elect[account_id] = Decimal
		(	working_map_elem.second,
			account->commodity()->precision()
		);
	}

	// Look for m_map elements for which the second is in an uninitialized
	// state. These must be the ones for which the Account has been deleted
	// from the database. These should then be removed from m_map.
	for (Map::iterator it = map_elect.begin(); it != map_elect.end(); ++it)
	{
		if (!it->second)
		{
			JEWEL_ASSERT (!Account::exists(m_database_connection, it->first));
			map_elect.erase(it);
		}
		else
		{
			JEWEL_ASSERT (Account::exists(m_database_connection, it->first));
		}
	}

	JEWEL_ASSERT (map_elect.size() == working_map.size());
	JEWEL_ASSERT (map_elect_ptr->size() == map_elect.size());
	using std::swap;
	swap(m_map, map_elect_ptr);
	return;
}

void
BalanceCache::refresh_targetted(vector<sqloxx::Id> const& p_targets)
{
	unique_ptr<Map> map_elect_ptr(new Map(*m_map));
	JEWEL_ASSERT (map_elect_ptr);
	Map& map_elect = *map_elect_ptr;
	for (auto const account_id: p_targets)
	{
		// Ordering by entry_id to decrease the likelihood of "intermediate
		// overflow" during calculation. Also consider impact on integrity
		// of PersistentJournal::would_cause_overflow().
		// TODO LOW PRIORITY Find out whether there is any point in this
		// ordering, given SQLite implementation.
		SQLStatement statement
		(	m_database_connection,
			"select sum(amount) from entries join ordinary_journal_detail "
			"using(journal_id) where account_id = :account_id order by entry_id"
		);
		statement.bind(":account_id", account_id);
		if (statement.step())
		{
			// NOTE If we can ever have multiple Commodities then this
			// will need to be changed.
			auto const precision =
				m_database_connection.default_commodity()->precision();
			try
			{
				auto const intval = statement.extract<Decimal::int_type>(0);
				map_elect[account_id] = Decimal(intval, precision);
			}
			catch (ValueTypeException&)
			{
				// There are no entries to sum.
				if (Account::exists(m_database_connection, account_id))
				{
					map_elect[account_id] = Decimal(0, precision);
				}
				else
				{
					// Account no longer exists in database, so should
					// be removed from the cache.

					// TODO LOW PRIORITY If we ever allow Accounts to be
					// deleted, then here we should test whether this passage
					// really is reached after removing
					// an Account from database. Note the removal of an
					// Account from the database should not have occurred
					// in the first place if there are any Entries in the
					// database that have this as their Account (due to the
					// foreign key constraints in the database); so that's why
					// this should \e should be reached as expected.
					Map::iterator doomed = map_elect.find(account_id);
					if (doomed != map_elect.end()) map_elect.erase(doomed);
				}
			}
			statement.step_final();
		}
		else
		{
			// There is always a result row even if it has null.
			JEWEL_HARD_ASSERT (false);
		}
	}
	using std::swap;
	swap(m_map, map_elect_ptr);
	return;
}

}  // namespace dcm

/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "account.hpp"
#include "date.hpp"
#include "balance_cache.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
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
	//
	// TODO LOW PRIORITY Would it be simpler, and almost as fast, to use
	// an EntryTableIterator here?

	// Bare scope
	{
		SQLStatement statement
		(	m_database_connection,
			"select account_id, amount from entries join "
			"ordinary_journal_detail using(journal_id)"
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
	// TODO Is this exception-safe?
	for (auto const account_id: p_targets)
	{
		Handle<Account> const account(m_database_connection, account_id);
		SQLStatement statement
		(	m_database_connection,
			"select sum(amount) from entries join ordinary_journal_detail "
			"using(journal_id) where account_id = :account_id"
		);
		statement.bind(":account_id", account_id);
		if (statement.step())
		{
			// TODO Catching exception here is a crappy way of telling whether
			// there are no entries to sum.
			try
			{
				(*m_map)[account_id] = Decimal
				(	statement.extract<Decimal::int_type>(0),
					account->commodity()->precision()
				);
			}
			catch (ValueTypeException&)
			{
				// There are no entries to sum.
				if (Account::exists(m_database_connection, account_id))
				{
					(*m_map)[account_id] =
						Decimal(0, account->commodity()->precision());
				}
				else
				{
					// Account no longer exists in database, so should
					// be removed from the cache.

					// TODO Test whether this really is reached after removing
					// an Account from database. Note the removal of an
					// Account from the database cannot possibly have occurred
					// in the first place if there are any Entries in the
					// database that have this as their Account (due to the
					// foreign key constraints in the database); so that's why
					// should \e should be reached as expected.
					Map::iterator doomed_iter = m_map->find(account_id);
					if (doomed_iter != m_map->end())  // TODO Is this check necessary?
					{
						m_map->erase(doomed_iter);
					}
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
	return;
}

}  // namespace phatbooks

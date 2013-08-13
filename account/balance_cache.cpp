// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account.hpp"
#include "account_impl.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "balance_cache.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "phatbooks_persistent_object.hpp"
#include "entry/entry_reader.hpp"
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/checked_arithmetic.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
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
using sqloxx::ValueTypeException;
using std::vector;

// For debugging only
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff

namespace phatbooks
{

BOOST_STATIC_ASSERT((boost::is_same<Account::Id, AccountImpl::Id>::value));

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
	if (m_map_is_stale)
	{
		refresh();
	}
	else
	{
		Map::iterator const it = m_map->find(p_account_id);

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
	return *(m_map->find(p_account_id)->second);
}

Decimal
BalanceCache::technical_opening_balance(AccountImpl::Id p_account_id)
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
	Account const account(m_database_connection, p_account_id);
	Decimal::places_type const places = account.commodity().precision();
	Decimal ret(0, places);
	if (statement.step())
	{
		// TODO Catching exception here is a crappy way of telling
		// whether there are no entries to sum.
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
		assert (false);
	}
	return ret;
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
	// Here we decide whether it's quicker to do a complete rebuild of
	// the entire cache, or whether it's quicker just to update for the
	// stale accounts. (Either way, we end
	// up with totally refreshed cache at the end - this is
	// purely an optimization decision.) The exact size of the optimal
	// fulcrum here is an educated guess made after some casual
	// experimentation. There is scope for further optimization if
	// required, by tweaking this fulcum figure.
	
	static vector<AccountImpl::Id>::size_type const fulcrum = 5;
	
	if (m_map_is_stale)
	{
		refresh_all();
		m_map_is_stale = false;
	}
	else
	{
		vector<AccountImpl::Id> stale_account_ids;
		SQLStatement statement
		(	m_database_connection,
			"select account_id from accounts"
		);
		Map::const_iterator const map_end = m_map->end();
		while (statement.step() && (stale_account_ids.size() != fulcrum))
		{
			AccountImpl::Id const account_id =
				statement.extract<AccountImpl::Id>(0);
			Map::const_iterator location_in_cache = m_map->find(account_id);
			if ((location_in_cache == map_end) || !location_in_cache->second)
			{
				// Either this AccountImpl::Id is not in the cache at all,
				// or it's in there but marked as stale.
				stale_account_ids.push_back(account_id);
			}
		}
		assert (stale_account_ids.size() <= fulcrum);
		if (stale_account_ids.size() == fulcrum)
		{
			refresh_all();
		}
		else
		{
			assert (stale_account_ids.size() < fulcrum);
			refresh_targetted(stale_account_ids);
		}
	}
	assert (!m_map_is_stale);
	return;
}

void
BalanceCache::refresh_all()
{
	typedef unordered_map<AccountImpl::Id, Decimal::int_type> WorkingMap;
	WorkingMap working_map;
	assert (working_map.empty());
	SQLStatement accounts_scanner
	(	m_database_connection,
		"select account_id from accounts"
	);
	while (accounts_scanner.step())
	{
		working_map[accounts_scanner.extract<AccountImpl::Id>(0)] = 0;
	}
	
	// It has been established that this is faster than using SQL
	// SUM and GROUP to sum Account totals. Note also that we have
	// chosen not to use an OrdinaryEntryReader here: we don't want
	// to load all the non-actual Entries into memory.

	// Bare scope
	{
		SQLStatement statement
		(	m_database_connection,
			"select account_id, amount from entries join "
			"ordinary_journal_detail using(journal_id)"
		);
		AccountImpl::Id account_id;
		Decimal::int_type amount_intval;
		while (statement.step())
		{
			account_id = statement.extract<AccountImpl::Id>(0);
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

	// Bare scope
	{
		WorkingMap::const_iterator it = working_map.begin();
		WorkingMap::const_iterator const end = working_map.end();
		for ( ; it != end; ++it)
		{
			AccountImpl::Id const account_id = it->first;
			Account const account(m_database_connection, account_id);
			map_elect[account_id] =
				Decimal(it->second, account.commodity().precision());
		}
	}

	// Look for m_map elements for which the second is in an uninitialized
	// state. These must be the ones for which the Account has been deleted
	// from the database. These should then be removed from m_map.
	for (Map::iterator it = map_elect.begin(); it != map_elect.end(); ++it)
	{
		if (!it->second)
		{
			assert (!Account::exists(m_database_connection, it->first));
			map_elect.erase(it);
		}
		else
		{
			assert (Account::exists(m_database_connection, it->first));
		}
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
	IdVec::const_iterator it = p_targets.begin();
	IdVec::const_iterator const end = p_targets.end();
	for ( ; it != end; ++it)
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
			// TODO Catching exception here is a crappy way of telling whether
			// there are no entries to sum.
			try
			{
				(*m_map)[account_id] = Decimal
				(	statement.extract<Decimal::int_type>(0),
					account.commodity().precision()
				);
			}
			catch (ValueTypeException&)
			{
				// There are no entries to sum.
				if (Account::exists(m_database_connection, account_id))
				{
					(*m_map)[account_id] =
						Decimal(0, account.commodity().precision());
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
			assert (false);
		}
	}
	return;
}

}  // namespace phatbooks

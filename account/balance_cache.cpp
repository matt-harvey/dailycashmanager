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
#include <sqloxx/sql_statement.hpp>
#include <algorithm>
#include <cassert>

using boost::optional;
using boost::scoped_ptr;
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
	(	"create view balance_view as "
		"select account_id, sum(amount) as balance "
		"from "
		"entries join ordinary_journal_detail using(journal_id) "
		"group by account_id;"
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
	// TODO Things are a bit confused here, in that it's not clear whether
	// we are conceptually dealing with a map of Account or a map
	// of AccountImpl. In practice it doesn't matter, but in it's
	// still a bit troubling.
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
	
	SQLStatement statement
	(	m_database_connection,
		"select account_id, balance from balance_view"
	);
	while (statement.step())
	{
		Account::Id const account_id = statement.extract<Account::Id>(0);
		Account const account(m_database_connection, account_id);
		map_elect[account_id] = Decimal
		(	statement.extract<Decimal::int_type>(1),
			account.commodity().precision()
		);
	}
	/** In due course we will may want to uncomment and
	 * implement this.
	// TODO Is this dangerous to get the local day? What if the
	// user is crossing between timezones?
	boost::gregorian::date const today = gregorian::date_clock::local_day();
	typedef PhatbooksDatabaseConnection::BudgetManagerAttorney;
	BudgetManagerAttorney::hypothetical_update
	(	m_database_connection,
		*map_elect,
		today
	);
	*/
	using std::swap;
	swap(m_map, map_elect_ptr);
	m_map_is_stale = false;
	return;
}



}  // namespace phatbooks

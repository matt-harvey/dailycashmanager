
/** \file phatbooks_database_connection.cpp
 *
 * \brief Source file pertaining to PhatbooksDatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "account_impl.hpp"
#include "commodity_impl.hpp"
#include "entry_impl.hpp"
#include "draft_journal_impl.hpp"
#include "ordinary_journal_impl.hpp"
#include "repeater_impl.hpp"


#include "account.hpp"
#include "balance_cache.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "draft_journal.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <sqloxx/sql_statement.hpp>
#include <sqlite3.h>
#include <stdexcept>
#include <boost/bimap.hpp>
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>


using boost::bimap;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::DatabaseConnection;
using sqloxx::DatabaseTransaction;
using sqloxx::IdentityMap;
using sqloxx::SQLStatement;
using sqloxx::SQLiteException;
using std::endl;
using std::list;
using std::runtime_error;
using std::string;



namespace phatbooks
{


PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
	DatabaseConnection(),
	m_balance_cache(0),
	m_account_map(0),
	m_commodity_map(0),
	m_entry_map(0),
	m_ordinary_journal_map(0),
	m_draft_journal_map(0),
	m_repeater_map(0)
{
	typedef PhatbooksDatabaseConnection PDC;
	m_balance_cache = new BalanceCache(*this);
	m_account_map = new IdentityMap<AccountImpl, PDC>(*this);
	m_commodity_map = new IdentityMap<CommodityImpl, PDC>(*this);
	m_entry_map = new IdentityMap<EntryImpl, PDC>(*this);
	m_ordinary_journal_map = new IdentityMap<OrdinaryJournalImpl, PDC>(*this);
	m_draft_journal_map = new IdentityMap<DraftJournalImpl, PDC>(*this);
	m_repeater_map = new IdentityMap<RepeaterImpl, PDC>(*this);
}

PhatbooksDatabaseConnection::~PhatbooksDatabaseConnection()
{
	delete m_balance_cache;
	delete m_account_map;
	delete m_commodity_map;
	delete m_entry_map;
	delete m_ordinary_journal_map;
	delete m_draft_journal_map;
	delete m_repeater_map;
}



bool
PhatbooksDatabaseConnection::has_account_named(string const& p_name)
{
	SQLStatement statement
	(	*this,
		"select name from accounts where name = :p"
	);
	statement.bind(":p", p_name);
	return statement.step();
}

bool
PhatbooksDatabaseConnection::has_draft_journal_named(string const& p_name)
{
	SQLStatement statement
	(	*this,
		"select name from draft_journal_detail where name = :p"
	);
	statement.bind(":p", p_name);
	return statement.step();
}

bimap<Account::AccountType, string>
PhatbooksDatabaseConnection::account_types()
{
	typedef bimap<Account::AccountType, string> return_type;
	return_type ret;
	SQLStatement statement
	(	*this,
		"select account_type_id, name from account_types order "
		"by account_type_id"
	);
	while (statement.step())
	{
		Account::AccountType acctype = static_cast<Account::AccountType>
		(	statement.extract<Account::Id>(0)
		);
		string accname = statement.extract<string>(1);
		ret.insert(return_type::value_type(acctype, accname));
	}
	return ret;
}



bool
PhatbooksDatabaseConnection::has_commodity_with_abbreviation
(	string const& p_abbreviation
)
{
	SQLStatement statement
	(	*this,
		"select abbreviation from commodities where abbreviation = :p"
	);
	statement.bind(":p", p_abbreviation);
	return statement.step();
}

bool
PhatbooksDatabaseConnection::has_commodity_named(string const& p_name)
{
	SQLStatement statement
	(	*this,
		"select name from commodities where name = :p"
	);
	statement.bind(":p", p_name);
	return statement.step();
}


void
PhatbooksDatabaseConnection::setup()
{
	if (setup_has_occurred())
	{
		return;
	}
	assert (!setup_has_occurred());
	DatabaseTransaction transaction(*this);
	setup_boolean_table();
	Commodity::setup_tables(*this);
	Account::setup_tables(*this);
	Journal::setup_tables(*this);
	DraftJournal::setup_tables(*this);
	OrdinaryJournal::setup_tables(*this);
	Repeater::setup_tables(*this);
	Entry::setup_tables(*this);
	mark_setup_as_having_occurred();
	transaction.commit();
	assert (setup_has_occurred());
	return;
}

void
PhatbooksDatabaseConnection::set_caching_level(unsigned int level)
{
	switch (level)
	{
	case 0: case 1: case 2: case 3: case 4:
		m_commodity_map->disable_caching();
		m_account_map->disable_caching();
		m_repeater_map->disable_caching();
		m_draft_journal_map->disable_caching();
		m_ordinary_journal_map->disable_caching();
		m_entry_map->disable_caching();
		break;
	case 5: case 6: case 7: case 8: case 9:
		m_commodity_map->enable_caching();
		m_account_map->enable_caching();
		m_repeater_map->disable_caching();
		m_draft_journal_map->disable_caching();
		m_ordinary_journal_map->disable_caching();
		m_entry_map->disable_caching();
		break;	
	case 10: default:
		assert (level > 0);
		m_commodity_map->enable_caching();
		m_account_map->enable_caching();
		m_repeater_map->enable_caching();
		m_draft_journal_map->enable_caching();
		m_ordinary_journal_map->enable_caching();
		m_entry_map->enable_caching();
		break;
	}
	return;
}


namespace
{
	string const setup_flag = "setup_flag_996149162";
}

void
PhatbooksDatabaseConnection::mark_setup_as_having_occurred()
{
	execute_sql("create table " + setup_flag + "(dummy_column);");
	return;
}


bool
PhatbooksDatabaseConnection::setup_has_occurred()
{
	try
	{
		execute_sql("select * from " + setup_flag);
		return true;
	}
	catch (SQLiteException&)
	{
		return false;
	}
}



// BalanceCacheAttorney

typedef
	PhatbooksDatabaseConnection::BalanceCacheAttorney
	BalanceCacheAttorney;


void
BalanceCacheAttorney::mark_as_stale
(	PhatbooksDatabaseConnection const& p_database_connection
)
{
	p_database_connection.m_balance_cache->mark_as_stale();
	return;
}


void
BalanceCacheAttorney::mark_as_stale
(	PhatbooksDatabaseConnection const& p_database_connection,
	AccountImpl::Id p_account_id
)
{
	p_database_connection.m_balance_cache->mark_as_stale
	(	p_account_id
	);
	return;
}


Decimal
BalanceCacheAttorney::technical_balance
(	PhatbooksDatabaseConnection const& p_database_connection,
	AccountImpl::Id p_account_id
)
{
	return p_database_connection.m_balance_cache->technical_balance
	(	p_account_id
	);
}


template <>
sqloxx::IdentityMap<AccountImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<AccountImpl>()
{
	return *m_account_map;
}

template <>
sqloxx::IdentityMap<EntryImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<EntryImpl>()
{
	return *m_entry_map;
}

template <>
sqloxx::IdentityMap<CommodityImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<CommodityImpl>()
{
	return *m_commodity_map;
}

template <>
sqloxx::IdentityMap<OrdinaryJournalImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<OrdinaryJournalImpl>()
{
	return *m_ordinary_journal_map;
}

template <>
sqloxx::IdentityMap<phatbooks::DraftJournalImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<DraftJournalImpl>()
{
	return *m_draft_journal_map;
}

template <>
sqloxx::IdentityMap<phatbooks::RepeaterImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<RepeaterImpl>()
{
	return *m_repeater_map;
}




}  // namespace phatbooks





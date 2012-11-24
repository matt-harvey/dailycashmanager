
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
#include "commodity.hpp"
#include "entry.hpp"
#include "draft_journal.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/identity_map.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/shared_sql_statement.hpp"
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
using sqloxx::SharedSQLStatement;
using sqloxx::SQLiteException;
using std::endl;
using std::list;
using std::runtime_error;
using std::string;



namespace phatbooks
{


PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
  DatabaseConnection()
{
}


bool
PhatbooksDatabaseConnection::has_account_named(string const& p_name)
{
	SharedSQLStatement statement
	(	*this,
		"select name from accounts where name = :p"
	);
	statement.bind(":p", p_name);
	return statement.step();
}

bool
PhatbooksDatabaseConnection::has_draft_journal_named(string const& p_name)
{
	SharedSQLStatement statement
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
	SharedSQLStatement statement
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
	SharedSQLStatement statement
	(	*this,
		"select abbreviation from commodities where abbreviation = :p"
	);
	statement.bind(":p", p_abbreviation);
	return statement.step();
}

bool
PhatbooksDatabaseConnection::has_commodity_named(string const& p_name)
{
	SharedSQLStatement statement
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
	begin_transaction();
	setup_boolean_table();
	Commodity::setup_tables(*this);
	Account::setup_tables(*this);
	Journal::setup_tables(*this);
	DraftJournal::setup_tables(*this);
	OrdinaryJournal::setup_tables(*this);
	Repeater::setup_tables(*this);
	Entry::setup_tables(*this);
	mark_setup_as_having_occurred();
	end_transaction();
	assert (setup_has_occurred());
	return;
}

void
PhatbooksDatabaseConnection::set_caching_level(unsigned int level)
{
	switch (level)
	{
	case 0: case 1: case 2: case 3: case 4:
		commodity_map().disable_caching();
		account_map().disable_caching();
		repeater_map().disable_caching();
		draft_journal_map().disable_caching();
		ordinary_journal_map().disable_caching();
		entry_map().disable_caching();
		break;
	case 5: case 6: case 7: case 8: case 9:
		commodity_map().enable_caching();
		account_map().enable_caching();
		repeater_map().disable_caching();
		draft_journal_map().disable_caching();
		ordinary_journal_map().disable_caching();
		entry_map().disable_caching();
		break;	
	case 10: default:
		assert (level > 0);
		commodity_map().enable_caching();
		account_map().enable_caching();
		repeater_map().enable_caching();
		draft_journal_map().enable_caching();
		ordinary_journal_map().enable_caching();
		entry_map().enable_caching();
		break;
	}
	return;
}

sqloxx::IdentityMap<AccountImpl>&
PhatbooksDatabaseConnection::account_map()
{
	return m_account_map;
}

sqloxx::IdentityMap<CommodityImpl>&
PhatbooksDatabaseConnection::commodity_map()
{
	return m_commodity_map;
}

sqloxx::IdentityMap<EntryImpl>&
PhatbooksDatabaseConnection::entry_map()
{
	return m_entry_map;
}

sqloxx::IdentityMap<OrdinaryJournalImpl>&
PhatbooksDatabaseConnection::ordinary_journal_map()
{
	return m_ordinary_journal_map;
}

sqloxx::IdentityMap<DraftJournalImpl>&
PhatbooksDatabaseConnection::draft_journal_map()
{
	return m_draft_journal_map;
}

sqloxx::IdentityMap<RepeaterImpl>&
PhatbooksDatabaseConnection::repeater_map()
{
	return m_repeater_map;
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


}  // namespace phatbooks

namespace sqloxx
{

template <>
IdentityMap<phatbooks::AccountImpl>&
identity_map
<	phatbooks::AccountImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.account_map();
}

template <>
IdentityMap<phatbooks::EntryImpl>&
identity_map
<	phatbooks::EntryImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.entry_map();
}

template <>
IdentityMap<phatbooks::CommodityImpl>&
identity_map
<	phatbooks::CommodityImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.commodity_map();
}

template <>
IdentityMap<phatbooks::OrdinaryJournalImpl>&
identity_map
<	phatbooks::OrdinaryJournalImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.ordinary_journal_map();
}

template <>
IdentityMap<phatbooks::DraftJournalImpl>&
identity_map
<	phatbooks::DraftJournalImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.draft_journal_map();
}

template <>
IdentityMap<phatbooks::RepeaterImpl>&
identity_map
<	phatbooks::RepeaterImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.repeater_map();
}

}  // namespace sqloxx





/** \file phatbooks_database_connection.cpp
 *
 * \brief Source file pertaining to PhatbooksDatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


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
	execute_sql("begin transaction");
	setup_boolean_table();
	Commodity::setup_tables(*this);
	Account::setup_tables(*this);
	Journal::setup_tables(*this);
	DraftJournal::setup_tables(*this);
	OrdinaryJournal::setup_tables(*this);
	Repeater::setup_tables(*this);
	Entry::setup_tables(*this);
	mark_setup_as_having_occurred();
	execute_sql("end transaction");
	assert (setup_has_occurred());
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

sqloxx::IdentityMap<Entry>&
PhatbooksDatabaseConnection::entry_map()
{
	return m_entry_map;
}

sqloxx::IdentityMap<OrdinaryJournal>&
PhatbooksDatabaseConnection::ordinary_journal_map()
{
	return m_ordinary_journal_map;
}

sqloxx::IdentityMap<DraftJournal>&
PhatbooksDatabaseConnection::draft_journal_map()
{
	return m_draft_journal_map;
}

sqloxx::IdentityMap<Repeater>&
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

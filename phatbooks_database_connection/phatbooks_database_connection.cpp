
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
#include "account_storage_manager.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "journal_storage_manager.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <boost/bimap.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <iostream>
#include <list>
#include <string>


using boost::bimap;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::DatabaseConnection;
using sqloxx::SQLStatement;
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
	SQLStatement statement
	(	*this,
		"select name from accounts where name = :p"
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
		(	statement.extract<IdType>(0)
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

	// Create the tables

	execute_sql("begin transaction");
	setup_boolean_table();
	Commodity::setup_tables(*this);
	Account::setup_tables(*this);
	setup_tables<Journal>();

	execute_sql
	(
		"create table interval_types"
		"("
			"interval_type_id integer primary key autoincrement, "
			"name text unique not null"
		"); "
		
		// Values inserted into interval_types must correspond with
		// IntervalType enum defined in Repeater class.
		"insert into interval_types(name) values('days'); "
		"insert into interval_types(name) values('weeks'); "
		"insert into interval_types(name) values('months'); "
		"insert into interval_types(name) values('month ends'); "

		"create table draft_journals"
		"("
			"draft_journal_id integer primary key autoincrement, "
			"is_actual integer not null references booleans, "
			"name text unique not null, "
			"comment text"
		"); "

		"create table draft_entries"
		"("
			"draft_entry_id integer primary key autoincrement, "
			"draft_journal_id not null references draft_journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null "
		"); "
	);


	execute_sql
	(	"create table " + s_setup_flag + "(dummy_column);"
		"end transaction;"
	);
	return;
}



bool
PhatbooksDatabaseConnection::setup_has_occurred()
{
	try
	{
		execute_sql("select * from " + s_setup_flag);
		return true;
	}
	catch (SQLiteException&)
	{
		return false;
	}
}




string const
PhatbooksDatabaseConnection::s_setup_flag = "setup_flag_99879871986";

}  // namespace phatbooks

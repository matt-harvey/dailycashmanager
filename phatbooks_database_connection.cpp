
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
#include "commodity_storage_manager.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <boost/bimap.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/debug_log.hpp>
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


IdType
PhatbooksDatabaseConnection::store(Journal const& p_journal)
{
	JEWEL_DEBUG_LOG << "Storing Journal object in database..." << endl;
	IdType const ret = next_auto_key<IdType>("journals");
	execute_sql("begin transaction;");
	SQLStatement statement
	(	*this,
		"insert into journals(is_actual, date, comment) "
		"values(:is_actual, :date, :comment)"
	);
	statement.bind(":is_actual", static_cast<int>(p_journal.is_actual()));
	statement.bind(":date", p_journal.date());
	statement.bind(":comment", p_journal.comment());
	statement.quick_step();

	// WARNING I should make this more efficient by setting up the 
	// account_id_finder and entry_storer outside the loop, and then
	// just resetting within the loop. Better, there could be a view with
	// some of this this stuff already worked out in the view, and I
	// could just query that view.

	typedef list< shared_ptr<Entry> > EntryCntnr;
	typedef EntryCntnr::const_iterator Iter;
	for
	(	Iter it = p_journal.m_entries.begin();
		it != p_journal.m_entries.end();
		++it
	)
	{
		SQLStatement account_id_finder
		(	*this,
			"select account_id from accounts where name = :aname"
		);
		account_id_finder.bind(":aname", (*it)->account_name());
		account_id_finder.step();
		IdType acct_id = account_id_finder.extract<IdType>(0);
		SQLStatement entry_storer
		(	*this,
			"insert into entries(journal_id, comment, account_id, "
			"amount) values(:journal_id, :comment, :account_id, :amount)"
		);
		entry_storer.bind(":journal_id", ret);
		entry_storer.bind(":comment", (*it)->comment());
		entry_storer.bind(":account_id", acct_id);
		entry_storer.bind(":amount", (*it)->amount().intval());
		entry_storer.quick_step();
	}
	execute_sql("end transaction;");
	JEWEL_DEBUG_LOG << "Journal object has been successfully stored." << endl;
	return ret;
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

Commodity
PhatbooksDatabaseConnection::commodity_for_account_named
(	string const& account_name
)
{
	SQLStatement statement
	(	*this,
		"select commodities.abbreviation, commodities.name, "
		"commodities.description, commodities.precision, "
		"commodities.multiplier_to_base_intval, "
		"commodities.multiplier_to_base_places "
		"from commodities join accounts using(commodity_id) "
		"where accounts.name = :p"
	);
	statement.bind(":p", account_name);
	statement.step();
	return Commodity
	(	statement.extract<string>(0),
		statement.extract<string>(1),
		statement.extract<string>(2),
		statement.extract<int>(3),
		Decimal
		(	statement.extract<Decimal::int_type>(4),
			numeric_cast<Decimal::places_type>
			(	statement.extract<boost::int64_t>(5)
			)
		)
	);
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
	JEWEL_DEBUG_LOG << "Setting up Phatbooks tables." << endl;


	execute_sql("begin transaction");

	execute_sql("create table booleans(representation integer primary key)");
	execute_sql("insert into booleans(representation) values(0)");
	execute_sql("insert into booleans(representation) values(1)");

	setup_tables<Commodity>();

	setup_tables<Account>();	

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

		"create table repeaters"
		"("
			"repeater_id integer primary key autoincrement, "
			"draft_journal_id not null references draft_journals, "
			"next_date text not null, "
			"interval_type_id references interval_types, "
			"interval_units integer not null"
		"); "

		"create table draft_entries"
		"("
			"draft_entry_id integer primary key autoincrement, "
			"draft_journal_id not null references draft_journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null "
		"); "

		"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"is_actual integer not null references booleans, "
			"date text not null, "
			"comment text"
		"); "

		"create table entries"
		"("
			"entry_id integer primary key autoincrement, "
			"journal_id not null references journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null "
		"); "

		"create table " + s_setup_flag + "(dummy_column);"

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

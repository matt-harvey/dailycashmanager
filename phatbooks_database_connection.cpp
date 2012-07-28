#include "account.hpp"
#include "phatbooks_database_connection.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <boost/numeric/conversion/cast.hpp>
#include <jewel/debug_log.hpp>
#include <iostream>
#include <string>

using boost::numeric_cast;
using sqloxx::DatabaseConnection;
using sqloxx::SQLiteException;
using std::endl;
using std::string;

namespace phatbooks
{


PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
  DatabaseConnection()
{
}


void
PhatbooksDatabaseConnection::store(Account const& p_account)
{
	JEWEL_DEBUG_LOG << "Storing Account object in database." << endl;

	SQLStatement statement
	(	*this,
		"insert into accounts(account_type_id, name, description) "
		"values(:account_type_id, :name, :description)"
	);

	statement.bind
	(	":account_type_id",
		static_cast<int>(p_account.account_type())
	);
	statement.bind(":name", p_account.name());
	statement.bind(":description", p_account.description());
     
	// Execute the SQL statement
	while (statement.step())
	{
	}
	JEWEL_DEBUG_LOG << "Account object has been successfully stored." << endl;
	return;
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
	execute_sql
	(	"begin transaction; "
		"create table commodities"
		"("
			"commodity_id integer primary key autoincrement, "
			"abbreviation text not null unique, "
			"name text unique, "
			"description text, "
			"precision integer default 2 not null, "
			"multiplier_to_base_intval integer not null, "
			"multiplier_to_base_places integer not null"
		"); "
		"create table account_types"
		"("
			"account_type_id integer primary key autoincrement, "
			"name text not null "
		"); "

		// Values inserted into account_types here must correspond
		// with AccountType enum defined Account class.
		"insert into account_types(name) values('profit and loss');"
		"insert into account_types(name) values('balance sheet');"
		"insert into account_types(name) values('pure envelope');"
		"create table interval_types"
		"("
			"interval_type_id integer primary key autoincrement, "
			"name text not null"
		"); "

		// Values inserted into interval_types must correspond with
		// IntervalType enum defined in Repeater class.
		"insert into interval_types(name) values('days');"
		"insert into interval_types(name) values('weeks');"
		"insert into interval_types(name) values('months');"
		"insert into interval_types(name) values('month ends');"

		"create table accounts"
		"("
			"account_id integer primary key autoincrement, "
			"account_type_id not null references account_types, "
			"name text not null unique, "
			"description text, "
			"commodity_id references commodities"
		"); "

		"create table draft_journals"
		"("
			"draft_journal_id integer primary key autoincrement, "
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
			"act_impact integer not null, "
			"bud_impact integer not null"
		"); "

		"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"date text not null, "
			"comment text"
		"); "

		"create table entries"
		"("
			"entry_id integer primary key autoincrement, "
			"journal_id not null references journals, "
			"comment text, "
			"account_id not null references accounts, "
			"act_impact integer not null, "
			"bud_impact integer not null"
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

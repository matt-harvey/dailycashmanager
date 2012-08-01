#include "account.hpp"
#include "commodity.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <boost/numeric/conversion/cast.hpp>
#include <jewel/debug_log.hpp>
#include <iostream>
#include <string>

using boost::numeric_cast;
using sqloxx::DatabaseConnection;
using sqloxx::SQLiteException;
using std::endl;
using std::runtime_error;
using std::string;



namespace phatbooks
{


PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
  DatabaseConnection()
{
}


IdType
PhatbooksDatabaseConnection::store(Commodity const& p_commodity)
{
	JEWEL_DEBUG_LOG << "Storing Commodity object in database." << endl;

	// Find the next key value
	IdType const ret = next_auto_key<IdType>("commodities");

	SQLStatement statement
	(	*this,
		"insert into commodities"
		"("
			"abbreviation, "
			"name, "
			"description, "
			"precision, "
			"multiplier_to_base_intval, "
			"multiplier_to_base_places"
		") "
		"values"
		"("
			":abbreviation, "
			":name, "
			":description, "
			":precision, "
			":multiplier_to_base_intval, "
			":multiplier_to_base_places"
		")"
	);

	statement.bind(":abbreviation", p_commodity.abbreviation());
	statement.bind(":name", p_commodity.name());
	statement.bind(":description", p_commodity.description());
	statement.bind(":precision", p_commodity.precision());
	statement.bind
	(	":multiplier_to_base_intval",
		p_commodity.multiplier_to_base().intval()
	);
	statement.bind
	(	":multiplier_to_base_places",
		p_commodity.multiplier_to_base().places()
	);
	// Execute the SQL statement
	statement.quick_step();
	JEWEL_DEBUG_LOG << "Commodity object has been successfully stored."
	                << endl;
	return ret;
}



IdType
PhatbooksDatabaseConnection::store(Account const& p_account)
{
	JEWEL_DEBUG_LOG << "Storing Account object in database." << endl;

	// Find the next auto_key
	IdType const ret = next_auto_key<IdType>("accounts");

	// Find the commodity_id for Account.commodity_abbreviation()
	SQLStatement commodity_finder
	(	*this,
		"select commodity_id from commodities where abbreviation = :ca"
	);
	commodity_finder.bind(":ca", p_account.commodity_abbreviation());
	if (!commodity_finder.step())
	{
		// We have no commodity stored with this abbreviation.
		throw runtime_error
		(	"Attempted to store Account with invalid commodity abbreviation."
		);
	}
	IdType const commodity_id = commodity_finder.extract<IdType>(0);
	if (commodity_finder.step())
	{
		// We have multiple commodities with this id.
		// This should never occur, unless the database has been tampered with
		// from outside this program.
		throw SQLiteException
		(	"Integrity of commodities table has been violated. Table contains"
			" multiple rows with the same commodity abbreviation."
		);
	}

	// Now we can insert the account.
	SQLStatement statement
	(	*this,
		"insert into accounts(account_type_id, name, description, "
		"commodity_id) values(:account_type_id, :name, :description, "
		":commodity_id)"
	);
	statement.bind
	(	":account_type_id",
		static_cast<int>(p_account.account_type())
	);
	statement.bind(":name", p_account.name());
	statement.bind(":description", p_account.description());
    statement.bind(":commodity_id", commodity_id);
	// Execute the SQL statement
	statement.quick_step();
	JEWEL_DEBUG_LOG << "Account object has been successfully stored." << endl;
	return ret;
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

		// Boolean values to act as foreign key constraint for other
		// table columns that must be 0 or 1.
		"create table booleans"
		"("
			"representation integer primary key"
		"); "
		"insert into booleans(representation) values(0); "
		"insert into booleans(representation) values(1); "

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
			"name text not null"
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
		"insert into interval_types(name) values('days'); "
		"insert into interval_types(name) values('weeks'); "
		"insert into interval_types(name) values('months'); "
		"insert into interval_types(name) values('month ends'); "

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

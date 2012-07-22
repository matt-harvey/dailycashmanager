#include "account.hpp"
#include "phatbooks_database_connection.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/numeric/conversion/cast.hpp>
#include <iostream>
#include <string>

using boost::numeric_cast;
using sqloxx::DatabaseConnection;
using sqloxx::SQLiteException;
using std::clog;
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
	clog << "Storing Account object in database." << endl;
	
	static string const sql_str =
	  "insert into accounts(account_type_id, name,"
	  " description) values(:account_type_id, :name, :description)";
	
	int return_code;
	sqlite3_stmt* sql_statement = 0;
	return_code = sqlite3_prepare_v2
	(	m_connection,
		sql_str.c_str(),
		-1,
		&sql_statement,
		0
	);
	if (return_code != SQLITE_OK)
	{
		sqlite3_finalize(sql_statement);
		sql_statement = 0;
		throw_sqlite_exception();
	}

	// Bind :account_type_id
	
	int const account_type_id_index = sqlite3_bind_parameter_index
	(	sql_statement,
		":account_type_id"
	);
	if (account_type_id_index == 0) throw_sqlite_exception();
	return_code = sqlite3_bind_int
	(	sql_statement,
		account_type_id_index,
		static_cast<int>(p_account.account_type())
	);
	if (return_code != SQLITE_OK)
	{
		sqlite3_finalize(sql_statement);
		sql_statement = 0;
		throw_sqlite_exception();
	}


	// Bind :name
	
	int const account_name_index = sqlite3_bind_parameter_index
	(	sql_statement,
	  	":name"
	);
	if (account_name_index == 0) throw_sqlite_exception();
	return_code = sqlite3_bind_text
	(	sql_statement,
		account_name_index,
		p_account.name().c_str(),
		-1,
		0
	);
	if (return_code != SQLITE_OK)
	{
		sqlite3_finalize(sql_statement);
		sql_statement = 0;
		throw_sqlite_exception();
	}
	                    
	// Bind :description
	
	int const description_index = sqlite3_bind_parameter_index(sql_statement,
	  ":description");
	if (description_index == 0) throw_sqlite_exception();
	return_code = sqlite3_bind_text
	(	sql_statement,
		description_index,
		p_account.description().c_str(),
		-1,
		0
	);
	if (return_code != SQLITE_OK)
	{
		sqlite3_finalize(sql_statement);
		sql_statement = 0;
		throw_sqlite_exception();
	}

	// Execute the SQL statement
	
	if (sqlite3_step(sql_statement) != SQLITE_DONE)
	{
		sqlite3_finalize(sql_statement);
		sql_statement = 0;
		throw_sqlite_exception();
	}
	
	// Clean up
	sqlite3_finalize(sql_statement);

	clog << "Account object has been successfully stored." << endl;
	return;
}




void
PhatbooksDatabaseConnection::setup_tables()
{
	// Create the tables
	int const return_code = sqlite3_exec
	(	m_connection,
		"begin transaction; "

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
		"insert into account_types(name) values('envelope');"
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

		"end transaction;",
		0,
		0,
		0
	);

	if (return_code != SQLITE_OK) throw_sqlite_exception();

	return;
}




}  // namespace phatbooks

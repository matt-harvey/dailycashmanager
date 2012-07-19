#include "session.hpp"
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

using std::clog;
using std::runtime_error;
using std::endl;

namespace phatbooks
{


Session::Session():
  m_database_connection(0)
{
	assert (m_database_connection == 0);
	clog << "Creating session..." << endl;
	sqlite3_initialize();
	clog << "SQLite3 has been initialized." << endl;
}


Session::~Session()
{
	clog << "Destroying session..." << endl;

	if (m_database_connection)
	{
		sqlite3_close(m_database_connection);
	}
	sqlite3_shutdown();

	clog << "SQLite3 has been shut down." << endl;
}


void
Session::activate_database(char const* filename)
{	
	bool database_setup_required = false;
	if (m_database_connection)
	{
		throw runtime_error("A database connection is already active");
	}
	
	boost::filesystem::path p(filename);
	boost::filesystem::file_status s = boost::filesystem::status(p);
	if (boost::filesystem::exists(s))
	{
		clog << "Preexisting file " << filename << " detected." << endl;
		return;
	}
	else
	{
		clog << "Creating file " << filename << "..." << endl;
		database_setup_required = true;
	}

	// Open the connection
	sqlite3_open_v2
	(
		filename,
		&m_database_connection,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		0
	);
	clog << "Database connection to file " << filename << " has been opened, "
	     << "and m_database_connection has been set to point there." << endl;
	
	if (database_setup_required)
	{
		create_database_tables();
	}
	return;
}


void
Session::create_database_tables()
{

	// Create the tables
	sqlite3_exec
	(
		m_database_connection,	
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
		"insert into account_types(name) values('profit and loss');"
		"insert into account_types(name) values('balance sheet');"
		"insert into account_types(name) values('envelope');"
		"create table interval_types"
		"("
			"interval_type_id integer primary key autoincrement, "
			"name text not null"
		"); "
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
			"commodity_id not null references commodities"
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
		"end transaction; ",
		0,
		0,
		0
	);

	return;
}




}  // namespace phatbooks

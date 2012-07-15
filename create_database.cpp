#include "create_database.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>

using std::vector;


namespace phatbooks
{


void create_database(char const* filename)
{
	// Declare pointer to sqlite3 database connection
	sqlite3* db;
	
	// Open the connection
	sqlite3_open_v2
	(
		filename,
		&db,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		0
	);

	// Create the tables
	sqlite3_exec
	(
		db,	
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
			"name text not null, "
			"description text"
		"); "
		"create table interval_types"
		"("
			"interval_type_id integer primary key autoincrement, "
			"name text not null"
		"); "
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

	// Close the connection
	sqlite3_close(db);

	return;
}

}  // namespace phatbooks

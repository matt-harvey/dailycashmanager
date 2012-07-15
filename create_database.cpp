#include "create_database.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>

using std::vector;


namespace phatbooks
{


void create_database(char const* filename)
{
	// Initialize SQLite3
	sqlite3_initialize();

	// Declare pointer to sqlite3 database connection
	sqlite3* db;
	
	// Open connection
	sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
	  0);

	// Create the SQL statements we need to create the tables
	
	char const* commodities_creator =
		"create table commodities\
		(\
			commodity_id integer primary key autoincrement, \
			abbreviation text not null unique, \
			name text unique, \
			description text, \
			precision integer default 2 not null, \
			multiplier_to_base_intval integer not null, \
			multiplier_to_base_places integer not null\
		)";
	
	char const* account_types_creator =
		"create table account_types\
		(\
			account_type_id integer primary key autoincrement, \
			name text not null, \
			description text\
		)";
	
	char const* interval_types_creator =
		"create table interval_types\
		(\
			interval_type_id integer primary key autoincrement, \
			name text not null\
		)";
	
	char const* accounts_creator =
		"create table accounts\
		(\
			account_id integer primary key autoincrement, \
			account_type_id not null references account_types, \
			name text not null unique, \
			description text, \
			commodity_id not null references commodities\
		)";
	
	char const* draft_journals_creator =
		"create table draft_journals\
		(\
			draft_journal_id integer primary key autoincrement, \
			name text unique not null, \
			comment text\
		)";
	
	char const* repeaters_creator =
		"create table repeaters\
		(\
			repeater_id integer primary key autoincrement, \
			draft_journal_id not null references draft_journals, \
			next_date text not null, \
			interval_type_id references interval_types, \
			interval_units integer not null\
		)";
	
	char const* draft_entries_creator =
		"create table draft_entries\
		(\
			draft_entry_id integer primary key autoincrement, \
			draft_journal_id not null references draft_journals, \
			comment text, \
			account_id not null references accounts, \
			act_impact integer not null, \
			bud_impact integer not null\
		)";
	
	char const* journals_creator =
		"create table journals\
		(\
			journal_id integer primary key autoincrement, \
			date text not null, \
			comment text\
		)";
	
	char const* entries_creator =
		"create table entries\
		(\
			entry_id integer primary key autoincrement, \
			journal_id not null references journals, \
			comment text, \
			account_id not null references accounts, \
			act_impact integer not null, \
			bud_impact integer not null\
		)";

	// Put our SQL statements on a vector
	vector< char const* > statements_to_execute;
	statements_to_execute.push_back(commodities_creator);
	statements_to_execute.push_back(account_types_creator);
	statements_to_execute.push_back(interval_types_creator);
	statements_to_execute.push_back(accounts_creator);
	statements_to_execute.push_back(draft_journals_creator);
	statements_to_execute.push_back(repeaters_creator);
	statements_to_execute.push_back(draft_entries_creator);
	statements_to_execute.push_back(journals_creator);
	statements_to_execute.push_back(entries_creator);

	// Execute the statements, thus creating the SQLite tables
	for (vector< char const* >::size_type i = 0;
	  i != statements_to_execute.size(); ++i)
	{
		sqlite3_exec(db, statements_to_execute[i], 0, 0, 0);
	}

	// Close connection to database
	sqlite3_close(db);

	// Shutdown SQLite3
	sqlite3_shutdown();

	return;
}

}  // namespace phatbooks

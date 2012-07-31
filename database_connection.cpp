#include "database_connection.hpp"
#include "sql_statement.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <jewel/debug_log.hpp>
#include <sqlite3.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::int64_t;
using std::clog;
using std::runtime_error;
using std::endl;
using std::string;
using std::vector;

namespace sqloxx
{


// Remember - don't call virtual functions from constructors!
DatabaseConnection::DatabaseConnection():
	m_connection(0)
{
	JEWEL_DEBUG_LOG << "Creating DatabaseConnection..." << endl;

	// Initialize SQLite3
	if (sqlite3_initialize() != SQLITE_OK) throw_sqlite_exception();

	JEWEL_DEBUG_LOG << "SQLite3 has been initialized." << endl;
}

void
DatabaseConnection::open(char const* filename)
{
	// Check if file already exists
	boost::filesystem::path p(filename);
	boost::filesystem::file_status s = boost::filesystem::status(p);
	if (boost::filesystem::exists(s))
	{
		JEWEL_DEBUG_LOG << "Preexisting file " << filename << " detected."
		                << endl;
		JEWEL_DEBUG_LOG << "Attempting to connect to this file..." << endl;
	}
	else
	{
		JEWEL_DEBUG_LOG << "Creating file " << filename << "..." << endl;
	}

	// Open the connection
	int const return_code = sqlite3_open_v2
	(	filename,
		&m_connection,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		0
	);
	if (return_code != SQLITE_OK) throw_sqlite_exception();
	JEWEL_DEBUG_LOG << "Database connection to file "
	                << filename
	                << " has been opened "
	                << "and m_connection has been set to point there."
					<< endl;

	setup();
	return;

}


// Remember - don't throw exceptions from destructors!
// Remember - don't call virtual functions from destructors!
DatabaseConnection::~DatabaseConnection()
{
	JEWEL_DEBUG_LOG << "Destroying database connection..." << endl;
	if (m_connection)
	{
		if (sqlite3_close(m_connection) != SQLITE_OK)
		{
			clog << "SQLite3 database connection could not be "
			             "successfully "
			             "closed in DatabaseConnection destructor. " << endl;
			std::abort();
		}
	}
	if (sqlite3_shutdown() != SQLITE_OK)
	{
		clog << "SQLite3 shutdown failed in DatabaseConnection destructor."
		     << endl;
		std::abort();
	}
	JEWEL_DEBUG_LOG << "SQLite3 has been shut down." << endl;
}

bool
DatabaseConnection::is_valid()
{
	return m_connection != 0;
}

void
DatabaseConnection::setup()
{
	// Do nothing.
	// This function should be overriden in inherited class
	// to perform application-specific database setup.
	return;
}

void
DatabaseConnection::throw_sqlite_exception()
{
	throw SQLiteException(sqlite3_errmsg(m_connection));
}

void
DatabaseConnection::execute_sql(string const& str)
{
	int const return_code = sqlite3_exec
	(	m_connection,
		str.c_str(),
		0,
		0,
		0
	);
	if (return_code != SQLITE_OK) throw_sqlite_exception();
	return;
}



vector<string>
DatabaseConnection::primary_key(string const& table_name)
{
	static int const pk_info_field = 5;
	static int const column_name_field = 1;
	vector<string> ret;
	SQLStatement statement
	(	*this,
		"pragma table_info(" + table_name + ")"
	);
	bool steps_remain = true;
	while ((steps_remain = statement.step()))
	{
		if (statement.extract<int>(pk_info_field) == 1)
		{
			ret.push_back(statement.extract<string>(column_name_field));
		}
	}
	assert (!steps_remain);
	return ret;
}


}  // namespace sqloxx

#include "sqloxx_exceptions.hpp"
#include "database_connection.hpp"
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using std::clog;
using std::runtime_error;
using std::endl;

namespace sqloxx
{


// Remember - don't call virtual functions from constructors!
DatabaseConnection::DatabaseConnection():
	m_connection(0)
{
	clog << "Creating DatabaseConnection..." << endl;

	// Initialize SQLite3
	if (sqlite3_initialize() != SQLITE_OK) throw_sqlite_exception();

	clog << "SQLite3 has been initialized." << endl;
}

void
DatabaseConnection::open(char const* filename)
{
	// Check if file already exists
	bool database_setup_required = false;
	boost::filesystem::path p(filename);
	boost::filesystem::file_status s = boost::filesystem::status(p);
	if (boost::filesystem::exists(s))
	{
		clog << "Preexisting file " << filename << " detected." << endl;
		clog << "Attempting to connect to this file..." << endl;
	}
	else
	{
		clog << "Creating file " << filename << "..." << endl;
		database_setup_required = true;
	}

	// Open the connection
	int const return_code = sqlite3_open_v2
	(	filename,
		&m_connection,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		0
	);
	if (return_code != SQLITE_OK) throw_sqlite_exception();
	clog << "Database connection to file " << filename << " has been opened, "
	     << "and m_connection has been set to point there." << endl;
	
	if (database_setup_required)
	{
		clog << "Setting up Phatbooks tables in database..." << endl;
		setup_tables();
		clog << "Tables have been set up." << endl;
	}
	return;

}


// Remember - don't throw exceptions from destructors!
// Remember - don't call virtual functions from destructors!
DatabaseConnection::~DatabaseConnection()
{
	clog << "Destroying database connection..." << endl;

	if (m_connection)
	{
		if (sqlite3_close(m_connection) != SQLITE_OK)
		{
			clog << "SQLite3 database connection could not be successfully "
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

	clog << "SQLite3 has been shut down." << endl;
}

bool
DatabaseConnection::is_valid()
{
	return m_connection != 0;
}

void
DatabaseConnection::setup_tables()
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


}  // namespace sqloxx

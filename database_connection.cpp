#include "sqloxx_exceptions.hpp"
#include "database_connection.hpp"
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

using std::clog;
using std::runtime_error;
using std::endl;
using std::string;

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



/**************************************************************
 * Now to define members of the nested class SQLStatement
 **************************************************************/


DatabaseConnection::SQLStatement::SQLStatement
(	DatabaseConnection& dbconn,
	string const& str
):
	m_statement(0),
	m_database_connection(dbconn)
{
	if (!dbconn.is_valid())
	{
		throw SQLiteException
		(	"Attempt to initialize SQLStatement with invalid "
			"DatabaseConnection."
		);
	}
	int const return_code = sqlite3_prepare_v2
	(	m_database_connection.m_connection,
		str.c_str(),
		-1,
		&m_statement,
		0
	);
	check_ok(return_code);
	return;
}


DatabaseConnection::SQLStatement::~SQLStatement()
{
	if (m_statement != 0)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
	}
}

void
DatabaseConnection::SQLStatement::check_ok(int err_code)
{
	if (err_code != SQLITE_OK)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
		m_database_connection.throw_sqlite_exception();
	}
	return;
}

void
DatabaseConnection::SQLStatement::bind
(	std::string const& parameter_name,
	int value
)
{
	int const index = parameter_index(parameter_name);
	int const return_code = sqlite3_bind_int(m_statement, index, value);
	check_ok(return_code);
	return;
}


void
DatabaseConnection::SQLStatement::bind
(	std::string const& parameter_name,
	std::string const& str
)
{
	int const index = parameter_index(parameter_name);
	int const return_code = sqlite3_bind_text
	(	m_statement,
		index,
		str.c_str(),
		-1,
		0
	);
	check_ok(return_code);
	return;
}
		

bool
DatabaseConnection::SQLStatement::step()
{
	int const return_code = sqlite3_step(m_statement);
	if (return_code == SQLITE_DONE)
	{
		return false;
	}
	if (return_code == SQLITE_ROW)
	{
		return true;
	}
	assert ( (return_code != SQLITE_DONE) && (return_code != SQLITE_ROW) );
	m_database_connection.throw_sqlite_exception();
	assert (false);  // Execution should never reach here.
	return false;  // Silence compiler re. return from non-void function. 
}

int
DatabaseConnection::SQLStatement::parameter_index
(	std::string const& parameter_name
)
const
{
	int const ret = sqlite3_bind_parameter_index
	(	m_statement,
		parameter_name.c_str()
	);
	if (ret == 0) 
	{
		throw SQLiteException("Could not find parameter index.");
	}
	assert (ret > 0);
	return ret;
}





}  // namespace sqloxx

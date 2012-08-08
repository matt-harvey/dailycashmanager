
/** \file database_connection.cpp
 *
 * \brief Source file pertaining to DatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */






#include "database_connection.hpp"
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
	if (sqlite3_initialize() != SQLITE_OK)
	{
		throw SQLiteInitializationError("SQLite could not be initialized.");
	}

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
	// Throw if already connected
	if (m_connection)
	{
		throw SQLiteException("Database already connected.");
	}
	// Open the connection
	sqlite3_open_v2
	(	filename,
		&m_connection,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		0
	);
	check_ok();
	JEWEL_DEBUG_LOG << "Database connection to file "
	                << filename
	                << " has been opened "
	                << "and m_connection has been set to point there."
					<< endl;
	execute_sql("pragma foreign_keys = on;");
	JEWEL_DEBUG_LOG << "Foreign key constraints enabled." << endl;
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
DatabaseConnection::check_ok()
{
	if (sqlite3_errcode(m_connection) == SQLITE_OK)
	{
		return;
	}
	assert (sqlite3_errcode(m_connection) != SQLITE_OK);
	char const* msg = sqlite3_errmsg(m_connection);
	switch (sqlite3_errcode(m_connection))
	{
	case SQLITE_ERROR:
		throw SQLiteError(msg);
		break;
	case SQLITE_INTERNAL:
		throw SQLiteInternal(msg);
		break;
	case SQLITE_PERM:
		throw SQLitePerm(msg);
		break;
	case SQLITE_ABORT:
		throw SQLiteAbort(msg);
		break;
	case SQLITE_BUSY:
		throw SQLiteBusy(msg);
		break;
	case SQLITE_LOCKED:
		throw SQLiteLocked(msg);
		break;
	case SQLITE_NOMEM:
		throw SQLiteNoMem(msg);
		break;
	case SQLITE_READONLY:
		throw SQLiteReadOnly(msg);
		break;
	case SQLITE_INTERRUPT:
		throw SQLiteInterrupt(msg);
		break;
	case SQLITE_IOERR:
		throw SQLiteIOErr(msg);
		break;
	case SQLITE_CORRUPT:
		throw SQLiteCorrupt(msg);
		break;
	case SQLITE_FULL:
		throw SQLiteFull(msg);
		break;
	case SQLITE_CANTOPEN:
		throw SQLiteCantOpen(msg);
		break;
	case SQLITE_EMPTY:
		throw SQLiteEmpty(msg);
		break;
	case SQLITE_SCHEMA:
		throw SQLiteSchema(msg);
		break;
	case SQLITE_TOOBIG:
		throw SQLiteTooBig(msg);
		break;
	case SQLITE_CONSTRAINT:
		throw SQLiteConstraint(msg);
		break;
	case SQLITE_MISMATCH:
		throw SQLiteMismatch(msg);
		break;
	case SQLITE_MISUSE:
		throw SQLiteMisuse(msg);
		break;
	case SQLITE_NOLFS:
		throw SQLiteNoLFS(msg);
		break;
	case SQLITE_AUTH:
		throw SQLiteAuth(msg);
		break;
	case SQLITE_FORMAT:
		throw SQLiteFormat(msg);
		break;
	case SQLITE_RANGE:
		throw SQLiteRange(msg);
		break;
	case SQLITE_NOTADB:
		throw SQLiteNotADB(msg);
		break;
	default:
		throw SQLiteUnknownErrorCode(msg);
	}
	assert (false);  // Execution should never reach here.
	return;
}

void
DatabaseConnection::execute_sql(string const& str)
{
	sqlite3_exec(m_connection, str.c_str(), 0, 0, 0);
	check_ok();
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


DatabaseConnection::SQLStatement::SQLStatement
(	DatabaseConnection& dbconn,
	string const& str
):
	m_statement(0),
	m_database_connection(dbconn)
{
	if (!dbconn.is_valid())
	{
		throw InvalidConnection
		(	"Attempt to initialize SQLStatement with invalid "
			"DatabaseConnection."
		);
	}
	sqlite3_prepare_v2
	(	m_database_connection.m_connection,
		str.c_str(),
		-1,
		&m_statement,
		0
	);
	check_ok();
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
DatabaseConnection::SQLStatement::check_column(int index, int value_type)
{
	int const num_columns = sqlite3_column_count(m_statement);
	if (num_columns == 0)
	{
		throw SQLiteException("Result row not available.");
	}
	if (index > num_columns)
	{
		throw SQLiteException("Index is out of range.");
	}
	if (index < 0)
	{
		throw SQLiteException("Index is negative.");
	}
	if (value_type != sqlite3_column_type(m_statement, index))
	{
		JEWEL_DEBUG_LOG << "Requested value type: " << value_type << endl;
		JEWEL_DEBUG_LOG << "Value type at index: "
		                << sqlite3_column_type(m_statement, index) << endl;
		throw SQLiteException
		(	"Value type at index does not match specified value type."
		);
	}
	return;
}


void
DatabaseConnection::SQLStatement::check_ok()
{
	try
	{
		m_database_connection.check_ok();
	}
	catch (SQLiteException&)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
		throw;
	}
	return;
}

void
DatabaseConnection::SQLStatement::bind
(	std::string const& parameter_name,
	int value
)
{
	sqlite3_bind_int(m_statement, parameter_index(parameter_name), value);
	check_ok();
	return;
}


void
DatabaseConnection::SQLStatement::bind
(	string const& parameter_name,
	int64_t value
)
{
	sqlite3_bind_int64(m_statement, parameter_index(parameter_name), value);
	check_ok();
	return;
}


void
DatabaseConnection::SQLStatement::bind
(	string const& parameter_name,
	string const& str
)
{
	sqlite3_bind_text
	(	m_statement,
		parameter_index(parameter_name),
		str.c_str(),
		-1,
		0
	);
	check_ok();
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
	check_ok();
	assert (false);  // Execution should never reach here.
	return false;  // Silence compiler re. return from non-void function. 
}


void
DatabaseConnection::SQLStatement::quick_step()
{
	if (step())
	{
		sqlite3_finalize(m_statement);
		throw UnexpectedResultSet
		(	"Statement yielded a result set when none was expected."
		);
	}
	return;
}

	


int
DatabaseConnection::SQLStatement::parameter_index
(	string const& parameter_name
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


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
#include "sql_statement.hpp"
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
DatabaseConnection::open(boost::filesystem::path const& filepath)
{
	if (filepath.string().empty())
	{
		throw InvalidFilename("Cannot open file with empty filename.");
	}
	// Check if file already exists
	if (boost::filesystem::exists(boost::filesystem::status(filepath)))
	{
		JEWEL_DEBUG_LOG << "Preexisting file \""
		                << filepath.string()
						<< "\" detected."
		                << endl;
		JEWEL_DEBUG_LOG << "Attempting to connect to this file."
		                << endl;
	}
	else
	{
		JEWEL_DEBUG_LOG << "Creating file \""
		                << filepath.string()
						<< "\"."
						<< endl;
	}
	// Throw if already connected or if filename is empty
	if (m_connection)
	{
		throw MultipleConnectionException("Database already connected.");
	}
	// Open the connection
	sqlite3_open_v2
	(	filepath.string().c_str(),
		&m_connection,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		0
	);
	check_ok();
	JEWEL_DEBUG_LOG << "Database connection to file \""
	                << filepath.string()
	                << "\" has been opened "
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
DatabaseConnection::is_valid() const
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
	// Assignment operator is deliberate here
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

void
DatabaseConnection::setup_boolean_table()
{
	execute_sql("create table booleans(representation integer primary key)");
	execute_sql("insert into booleans(representation) values(0)");
	execute_sql("insert into booleans(representation) values(1)");
}

}  // namespace sqloxx

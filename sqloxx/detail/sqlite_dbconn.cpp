
/** \file sqlite_dbconn.cpp
 *
 * \brief Source file pertaining to SQLiteDBConn class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */






#include "sqlite_dbconn.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::int64_t;
using std::abort;
using std::clog;
using std::endl;
using std::runtime_error;
using std::string;
using std::vector;


namespace sqloxx
{
namespace detail
{


SQLiteDBConn::SQLiteDBConn():
	m_connection(0)
{
	// Initialize SQLite3
	if (sqlite3_initialize() != SQLITE_OK)
	{
		throw SQLiteInitializationError("SQLite could not be initialized.");
	}

}

void
SQLiteDBConn::open(boost::filesystem::path const& filepath)
{
	if (filepath.string().empty())
	{
		throw InvalidFilename("Cannot open file with empty filename.");
	}
	// Check if file already exists
	// While this conditional doesn't do anything, it is retained here
	// for convenience in case logging or other message required.
	if (boost::filesystem::exists(boost::filesystem::status(filepath)))
	{
		// Preexisting filepath detected... 
	}
	else
	{
		// New file created...
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
	execute_sql("pragma foreign_keys = on;");
	return;

}


// Remember - don't throw exceptions from destructors!
// Remember - don't call virtual functions from destructors!
SQLiteDBConn::~SQLiteDBConn()
{

	if (m_connection)
	{
		if (sqlite3_close(m_connection) != SQLITE_OK)
		{
			clog << "SQLite3 database connection could not be "
			             "successfully "
			             "closed in SQLiteDBConn destructor. " << endl;
			abort();
		}
	}
	if (sqlite3_shutdown() != SQLITE_OK)
	{
		clog << "SQLite3 shutdown failed in SQLiteDBConn destructor."
		     << endl;
		abort();
	}
}

bool
SQLiteDBConn::is_valid() const
{
	return m_connection != 0;
}

void
SQLiteDBConn::check_ok()
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
SQLiteDBConn::execute_sql(string const& str)
{
	sqlite3_exec(m_connection, str.c_str(), 0, 0, 0);
	check_ok();
	return;
}



}  // namespace detail
}  // namespace sqloxx

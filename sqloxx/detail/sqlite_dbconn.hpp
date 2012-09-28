#ifndef GUARD_sqlite_dbconn_hpp
#define GUARD_sqlite_dbconn_hpp

/** \file sqlite_dbconn.hpp
 *
 * \brief Header file pertaining to SQLiteDBConn class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 *
 * @todo Move sqloxx code to a separate library.
 * 
 * @todo Make the SQLiteDBConn class provide for a means
 * to check whether the particular file being connected to is
 * the right kind of file for the application that is using
 * the SQLiteDBConn class. At the moment it just checks whether
 * the file exists. This could involve overriding some method or other.
 *
 * @todo There should probably be a close method. Even though the
 * connection is closed by the destructor, there should probably
 * be a way of closing it independently, so that it can be connected
 * elsewhere.
 *
 * @todo Consider supplying public member function to close any
 * database connections and shut down SQLite3. Current this is done
 * in the destructor, but this can't throw.
 *
 * @todo SQLiteDBConn::is_valid should probably do more than
 * just check whether m_connection exists. It should probably also
 * at least check SQLite error status.
 *
 * @todo I am often calling sqlite3_finalize() before throwing
 * SQLiteException(). Is this necessary? Is this desirable? What
 * about if reset() is preferred by the caller?
 */


#include "sqloxx/sqloxx_exceptions.hpp"
#include "sql_statement.hpp"
#include <jewel/checked_arithmetic.hpp>
#include <sqlite3.h>
#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <limits>
#include <string>
#include <vector>



namespace sqloxx
{
namespace detail
{



/**
 * @class SQLiteDBConn
 *
 * This class is designed to be used only by other classes within
 * the Sqloxx module, and should not be accessed by external client
 * code.
 *
 * This class is intended to encapsulate calls to the C API
 * provided by SQLite, in relation to managing a database connection
 * represented by a sqlite3* struct.
 *
 * SQLiteDBConn is in designed to be contained
 * within an instance of DatabaseConnection; and it is DatabaseConnection
 * that provides the higher-level interface with clients outside of
 * Sqloxx, as well as a range of convenience functions that are not
 * provided by this lower-level class.
 */
class SQLiteDBConn:
	private boost::noncopyable
{
	friend class SQLStatement;

public:


	/**
	 * Initializes SQLite3 and creates a database connection
	 * initially set to null.
	 *
	 * @throws SQLiteInitializationError if initialization fails
	 * for any reason.
	 */
	SQLiteDBConn();

	/**
	 * Closes any open SQLite3 database connection, and also
	 * shuts down SQLite3.
	 *
	 * Does not throw. If SQLite3 connection closure or shutdown fails,
	 * the application is aborted with a diagnostic message written to
	 * std::clog.
	 */
	~SQLiteDBConn();

	/**
	 * Returns \c true iff the SQLiteDBConn is connected to a 
	 * database. Does not throw.
	 */
	bool is_valid() const;

	/**
	 * Implements DatabaseConnection::open.
	 */
	void open(boost::filesystem::path const& filepath);	

	/**
	 * Implements DatabaseConnection::execute_sql
	 */
	void execute_sql(std::string const& str);

	/**
	 * Implements DatabaseConnection::check_ok
	 */
	void check_ok();


private:

	
	/**
	 * A connection to a SQLite3 database file.
	 *
	 * (Note this is a raw pointer not a smart pointer
	 * to facilitate more straightforward interaction with the SQLite
	 * C API.)
	 */
	sqlite3* m_connection;


};




}  // namespace detail
}  // namespace sqloxx

#endif  // GUARD_sqlite_dbconn_hpp

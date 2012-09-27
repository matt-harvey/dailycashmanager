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


#include "sqloxx_exceptions.hpp"
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




/**
 * Namespace for housing all Sqloxx code. Sqloxx is intended as a
 * thin wrapper around SQLite, to facilitate using SQLite from C++,
 * to facilitate managing the resources required by SQLite using RAII,
 * and to reduce the verbosity required in client code wishing to
 * using the SQLite library. Sqloxx will not necessarily provide
 * much in the way of objected-relational mapping. The intention is
 * that the C++ API of Sqloxx will largely mirror the C API of
 * SQLite, so that Sqloxx could be used easily by anyone who is
 * familiar with SQLite (and with C++).
 */
namespace sqloxx
{



/**
 * Class to manage connection to SQLite3 database and execute
 * SQL on that database. 
 *
 * This class can be derived from and certain member functions
 * overridden to create application-specific database code.
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
	virtual ~SQLiteDBConn();

	/**
	 * Returns \c true iff the SQLiteDBConn is connected to a 
	 * database. Does not throw.
	 */
	virtual bool is_valid() const;

	/**
	 * Points the database connection to a specific file
	 * given by \c filename. If the file
	 * does not already exist it is created. Note the SQLite pragma
	 * foreign_keys is always executed immediately the file is opened, to
	 * enable foreign key constraints.
	 *
	 * @todo This should be made to support Unicode filepaths, which
	 * apparently are used on Windows.
	 *
	 * @todo It appears that boost::filesystem::path::string() produces
	 * a GENERIC string (safely passable to SQLite database connection
	 * opening function) in Boost Version 1.42; but that in Version 1.46
	 * this produces a NATIVE string! Currently this function relies on the
	 * behaviour in version 1.42. I should use a macro or something to
	 * make it portable between versions of Boost.
	 *
	 * @param filepath File to connect to. The is in the form of a
	 * \c boost::filesystem::path to facilitate portability.
	 *
	 * @todo Do a full portability test to Windows, especially for cases
	 * involving escape characters and such.
	 *
	 * @throws sqloxx::InvalidFilename if filename is an empty string.
	 *
	 * @throws sqloxx::MultipleConnectionException if already connected to a
	 * database (be it this or another database).
	 *
	 * @throws SQLiteException or an exception derived therefrom (likely, but
	 * not guaranteed, to be SQLiteCantOpen) if for some other reason the
	 * connection cannot be opened.
	 */
	void open(boost::filesystem::path const& filepath);	

	/**
	 * Executes a string on the database connection.
	 * This should be used only where the developer has complete
	 * control of the string being passed, to prevent SQL injection
	 * attacks. Generally, the functions provided by SQLStatement should
	 * be the preferred means for building and executing SQL statements.
	 *
	 * @throws SQLiteException or some exception inheriting thereof, whenever
	 * there is any kind of error executing the statement.
	 */
	void execute_sql(std::string const& str);



	/**
	 * If the database connection is in an error state recognized by SQLite,
	 * this throws a \c SQLiteException with the current sqlite3_errmsg passed
	 * to the constructor of the exception. The exact exception thrown
	 * corresponds to the current SQLite error code for the connection. Any
	 * thrown exception will be an instance of class that is, or extends,
	 * \c SQLiteException.
	 *
	 * @todo If error code is SQLITE_DONE or SQLITE_ROWS, this throws
	 * SQLiteUnknownErrorCode. Improve this behaviour.
	 */
	void check_ok();

	/*
	 * Creates table containing integers representing boolean values.
	 * This might be used to provide foreign key constraints for other
	 * tables where we wish a particular column to have boolean values
	 * only.
	 *
	 * The table is called "booleans" and has one column, an integer
	 * primary key field with the heading "representation". There are
	 * two rows, one with 0 in the "representation" column, representing
	 * \e false, and the other with 1, representing \e true.
	 */
	void setup_boolean_table();



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





}  // namespace sqloxx

#endif  // GUARD_sqlite_dbconn_hpp

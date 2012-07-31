#ifndef GUARD_database_connection_hpp
#define GUARD_database_connection_hpp

#include "sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <cassert>
#include <string>
#include <vector>

/**
 *
 * @todo Move sqloxx code to a separate library.
 * 
 * @todo Make the DatabaseConnection class provide for a means
 * to check whether the particular file being connected to is
 * the right kind of file for the application that is using
 * the DatabaseConnection class. At the moment it just checks whether
 * the file exists. This could involve overriding some method or other.
 *
 * @todo Use boost::filesystem::path to make filepath passed to
 * activate_database more portable.
 *
 * @todo Consider supplying public member function to close any
 * database connections and shut down SQLite3. Current this is done
 * in the destructor, but this can't throw.
 *
 * @todo DatabaseConnection::is_valid should probably do more than
 * just check whether m_connection exists. It should probably also
 * at least check SQLite error status.
 *
 * @todo I am often calling sqlite3_finalize() before throwing
 * SQLiteException(). Is this necessary? Is this desirable? What
 * about if reset() is preferred by the caller?
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
class DatabaseConnection:
	private boost::noncopyable
{
public:


	/**
	 * Initializes SQLite3 and creates a database connection
	 * initially set to null.
	 *
	 * @throws SQLiteException if SQLite3 initialization fails,
	 * or if database connection cannot be opened.
	 */
	DatabaseConnection();

	/**
	 * Closes any open SQLite3 database connection, and also
	 * shuts down SQLite3.
	 *
	 * Does not throw. If SQLite3 connection closure or shutdown fails,
	 * the application is aborted with a diagnostic message written to
	 * std::clog.
	 */
	virtual ~DatabaseConnection();

	/**
	 * Returns \c true iff the DatabaseConnection is connected to a 
	 * database.
	 */
	virtual bool is_valid();

	/**
	 * Points the database connection to a specific file
	 * given by \c filename. If the file
	 * does not already exist it is created. After creation, a separate
	 * \c setup function is called, to set up
	 * any application specific tables or etc. and initial data as desired.
	 *
	 * To customise this "setup" in a derived class, override
	 * \c DatabaseConnection::setup (which does nothing by default)
	 * in your class derived from DatabaseConnection.
	 *
	 * \c DatabaseConnection::setup has the signature:\n
	 * 	<tt> virtual void setup() </tt>\n
	 * and is private. It should throw SQLiteException in case it fails.
	 *
	 * @param filename file to connect to
	 *
	 * @throws SQLiteException if SQLite3
	 * if database connection cannot be opened to the specified file,
	 * or if a new file is created but \c setup does not
	 * succeed.
	 */
	void open(char const* filename);	


protected:

	/**
	 * Wrapper class for sqlite_stmt*.
	 *
	 * @todo The constructor to create a SQLStatement should reject strings
	 * containing semicolons, since compound statements are not handled by
	 * step() properly. There should be some other class SQLMultiStatement or
	 * something, which can then executed using a wrapper around sqlite3_exec.
	 */
	class SQLStatement;


	/**
	 * Throws a SQLiteException with the current sqlite3_errmsg passed
	 * to the constructor of the exception.
	 *
	 * This is essentially to save typing.
	 *
	 * @throws SQLiteException whenever called
	 */
	void throw_sqlite_exception();

	/**
	 * Executes a string on the database connection.
	 * This should be used only where the developer has complete
	 * control of the string being passed, to prevent SQL injection
	 * attacks. Generally, the functions provided by SQLStatement should
	 * be the preferred means for building and executing SQL statements.
	 */
	void execute_sql(std::string const& str);

	/**
	 * To find primary key of a table.
	 *
	 * Note, this function should not be used where \c table_name is
	 * an untrusted string.
	 *
	 * @todo To speed execution, assuming the return value for a given
	 * \c table_name never changes (is this a safe assumption?), the
	 * return values could be cached either in a
	 * map< string, vector<string> >, or by way of templatizing this
	 * function with \c table_name as a (non-type) template parameter,
	 * and the return value stored as a static variable inside the
	 * function.
	 *
	 * @returns a vector of the names of all the columns making up the
	 * primary key of the table named \c table_name. An empty vector
	 * is returned if there is no primary key.
	 *
	 * @throws SQLiteException in case of invalid table name or other
	 * SQL execution error.
	 *
	 * @param table_name name of table
	 */
	std::vector<std::string> primary_key(std::string const& table_name);

	
private:

	/**
	 * A connection to a SQLite3 database file.
	 *
	 * (Note this is a raw pointer not a smart pointer
	 * to facilitate more straightforward interaction with the SQLite
	 * C API.)
	 */
	sqlite3* m_connection;

	/**
	 * Create application-specific tables or perform other setup
	 * when database is opened.
	 *
	 * This function should be redefined in derived class if required
	 * to perform application-specific database setup.
	 *
	 * @throws sqloxx::SQLiteException if operation
	 * fails.
	 */
	virtual void setup();

};




}  // namespace sqloxx

#endif  // GUARD_database_connection_hpp

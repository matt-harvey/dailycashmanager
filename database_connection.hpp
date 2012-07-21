#ifndef GUARD_database_connection_hpp
#define GUARD_database_connection_hpp

#include <sqlite3.h>
#include <boost/utility.hpp>

/**
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
	 * Points the datase connection to a specific file
	 * given by \c filename. If the file
	 * does not already exist it is created, and is populated with
	 * the tables required for Phatbooks. Certain tables
	 * containing specific "fixed" application data are populated
	 * with data in this process. (When inheriting from this class, you
	 * should override setup_tables() to have it peform the application-
	 * specific setup code you require on the database.)
	 *
	 * @param filename file to connect to
	 *
	 * @throws SQLiteException if SQLite3
	 * if database connection cannot be opened to the specified file,
	 * or if a new file is created but setup of Phatbooks tables does not
	 * succeed.
	 */
	void open(char const* filename);	

protected:
	
	/**
	 * A connection to a SQLite3 database file.
	 *
	 * (Note this is a raw pointer not a smart pointer
	 * to facilitate more straightforward interaction with the SQLite
	 * C API.)
	 */
	sqlite3* m_connection;

	/**
	 * Create application-specific tables in database.
	 * Certain tables containing specific "fixed" application data are
	 * populated with rows in this process.
	 *
	 * This function should be overriden in derived class if required
	 * to perform application-specific database setup.
	 *
	 * @throws sqloxx::SQLiteException if operation
	 * fails.
	 */
	virtual void setup_tables();

};

}  // namespace sqloxx

#endif  // GUARD_database_connection_hpp

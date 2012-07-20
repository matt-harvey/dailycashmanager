#ifndef GUARD_database_connection_hpp
#define GUARD_database_connection_hpp

#include <sqlite3.h>
#include <boost/utility.hpp>

/**
 * @todo Make DatabaseConnection::open(const char*) check both for
 * existence of file, and to see whether the file if it does
 * exist is a Phatbooks-specific SQLite3 database file, or whether
 * it's just some file that happens to have the same name.
 *
 * @todo Use boost::filesystem::path to make filepath passed to
 * activate_database more portable.
 *
 * @todo Consider supplying public member function to close any
 * database connections and shut down SQLite3. Current this is done
 * in the destructor, but this can't throw.
 *
 */




namespace phatbooks
{



class DatabaseConnection: private boost::noncopyable
{
public:
	/**
	 * Initializes SQLite3, and opens a database connection
	 * to a SQLite3 database file \c filename. If the file
	 * does not already exist it is created, and is populated with
	 * the tables required for Phatbooks. Certain tables
	 * containing specific "fixed" application data are populated
	 * with rows in this process.
	 *
	 * @param filename file to connect to
	 *
	 * @throws SQLiteException if SQLite3 initialization fails, or
	 * if database connection cannot be opened, or if a new file
	 * is created but setup of Phatbooks tables does not succeed.
	 *
	 */
	DatabaseConnection(char const* filename);

	/**
	 * Closes any open SQLite3 database connection, and also
	 * shuts down SQLite3.
	 *
	 * Does not throw. If SQLite3 connection closure or shutdown fails,
	 * the application is aborted with a diagnostic message written to
	 * std::clog.
	 */
	~DatabaseConnection();

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
	 * Create Phatbooks-specific tables in database.
	 * Certain tables containing specific "fixed" application data are
	 * populated with rows in this process.
	 *
	 * @throws phatbooks::SQLiteException if operation
	 * fails.
	 */
	void create_phatbooks_tables();

};

}  // namespace phatbooks

#endif  // GUARD_database_connection_hpp

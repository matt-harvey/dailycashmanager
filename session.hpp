#ifndef GUARD_session_hpp
#define GUARD_session_hpp

#include <sqlite3.h>
#include <boost/utility.hpp>

/**
 * @todo Make Session::activate_database check both for
 * existence of file, and to see whether the file if it does
 * exist is a Phatbooks-specific SQLite3 database file, or whether
 * it's just some file that happens to have the same name.
 *
 * @todo Use boost::filesystem::path to make filepath passed to
 * activate_database more portable.
 *
 * @todo Check return values from SQLite3-interfacing functions.
 * Throw exceptions where applicable.
 */


namespace phatbooks
{

/** Class to manage resource associated with a user session
 */


class Session: private boost::noncopyable
{
public:
	/**
	 * Starts a Phatbooks user session.
	 * Initializes SQLite3 (but does not create database
	 * connection).
	 */
	Session();

	/**
	 * Ends a Phatbooks user session.
	 * Shuts down SQLite3. (It is assumed that there are no
	 * open connections at this point.)
	 */
	~Session();

	/**
	 * Activates database connection to database file.
	 * File will be created if it doesn't already exist.
	 *
	 * @param filename char const* filename.
	 *
	 * @throws std::runtime_error if a connection is
	 * already active.
	 */
	void activate_database(char const* filename);

private:
	
	// I can't get this to work with smart pointers, due
	// to the need to pass the address of a raw pointer
	// to the sqlite3_close function.
	sqlite3* m_database_connection;

	/**
	 * Create tables in database.
	 */
	void create_database_tables();

};

}  // namespace phatbooks

#endif  // GUARD_session_hpp

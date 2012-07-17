#ifndef GUARD_session_hpp
#define GUARD_session_hpp

#include <sqlite3.h>
#include <iostream>

namespace phatbooks
{

class Session
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

private:

};


inline Session::Session()
{
	std::clog << "Creating session..." << std::endl;
	sqlite3_initialize();
	std::clog << "SQLite3 has been initialized." << std::endl;
}

inline Session::~Session()
{
	std::clog << "Destroying session..." << std::endl;
	sqlite3_shutdown();
	std::clog << "SQLite3 has been shut down." << std::endl;
}


}  // namespace phatbooks

#endif  // GUARD_session_hpp

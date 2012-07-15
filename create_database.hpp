#ifndef GUARD_create_database_hpp
#define GUARD_create_database_hpp


/**
 * 
 * @todo Add error checking to the create_database function, that tests
 * return values of the C Sqlite API functions, then throws C++ exceptions
 * where these return error values.
 *
 * @todo Unit test the create_database function.
 * 
 * @todo Make the database accord more closely with the intended structure
 * of Phatbooks. The differs from the N. A. P. -like structure currently
 * exhibited by the created database, in that Envelopes are in fact a
 * separate type of account. Note this is a "medium term" action - for the
 * time being, I just want to get SQLite working with C++.
 *
 * @todo Ensure create_database will work cross-platform.
 */

namespace phatbooks
{

/**
 * This function creates a SQLite database and creates therein the
 * tables required for Phatbooks.
 * 
 * It is the callers responsibility to call sqlite3_initialize
 * and sqlite3_shutdown at some stage before, and at some stage
 * after, respectively, calling this function. However, the
 * create_database function takes care of opening and closing the
 * connection to the database.
 *
 * @param filename is a char const* being the entire
 * absolute Unix-style filepath including the filename
 * and extension, where you want the database to be stored.
 * The file will be created if it does not already exist, and will
 * be overwritten if it does already exist.
 */
void create_database(char const* filename);


}  // namespace phatbooks
#endif  // GUARD_create_database_hpp

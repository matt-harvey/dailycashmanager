#ifndef GUARD_phatbooks_database_connection_hpp
#define GUARD_phatbooks_database_connection_hpp

#include "account.hpp"
#include "commodity.hpp"
#include "database_connection.hpp"

namespace phatbooks
{

/** Phatbooks-specific database connection class.
 * See API documentation for sqloxx::DatabaseConnection,
 * for parts of API inherited from sqloxx::DatabaseConnection.
 *
 * @todo If speed becomes a problem, I should find a way to avoid
 * having to call sqlite3_prepare_v2, by caching previously prepared
 * sqlite3_stmt* somehow. I think the only feasible way would be to store it
 * as a static member
 * variable of DatabaseConnection. That way the sqlite3_stmt can
 * be freed using sqlite3_finalize in the destructor.
 * There would end up being several of the pre-prepared statements.
 * I tried making the prepared SQL statement a static variable in the
 * \c function, but this meant I couldn't call sqlite3_finalize on the
 * statement at the appropriate time.
 * Having said this, there's no point unless
 * it causes a problem by being too slow.
 *
 * @todo I should do the following:
 *	Have the PhatbooksDatabaseConnection::store(T const& obj) methods first
 *	check the maximum of the autoincrementing primary key column for their
 *	particular table, to ensure it is not INT_MAX; then have them store
 *	the result of that maximum plus one, in a local variable \ret; then
 *	after successful completion of the storage
 *	operation, have them return \ret from the \c store function.
 *	This could be perhaps be done using the NVI idiom, to move
 *	this shared "bookkeeping" code to the base class DatabaseConnection.
 *	This will only work for tables that have an autoincrementing primary
 *	key column.
 *	I could do this more simply by just having a normal protected function
 *	provided by DatabaseConnection, which you pass a table name and a column
 *	name, and it (a) checks that the maximum of that column can be safely
 *	incremented, (b) perhaps checks that that column is an autoincrementing
 *	primary key, and (c) returns the value one more that the maximum of that
 *	column. The \c store functions would then just manually call this
 *	function before inserting... They would have to remember to do this
 *	though...
 *
 *
 */
class PhatbooksDatabaseConnection:
	public sqloxx::DatabaseConnection
{
public:
	PhatbooksDatabaseConnection();
	
	/** Store an Account object in the database
	 * 
	 * @todo Verify that throwing behaviour is as documented.
	 * 
	 * @throws std::runtime_error if p_account has invalid
	 * commodity abbreviation. (Commodity abbreviation of
	 * p_account must correspond to a commodity that has already
	 * been stored in the database.)
	 *
	 * @throws sqloxx::SQLiteException if:\n
	 * 	SQLite statement preparation fails; or\n
	 * 	Insertion into database would cause the (auto-incrementing)
	 * 	primary key to overflow IdType; or\n
	 * 	Insertion fails (for reasons other than above).
	 *
	 * @param p_account the Account to be stored.
	 *
	 * @returns the integer primary key of the Account just stored.
	 * (This is an autoincrementing primary key.)
	 */
	IdType store(Account const& p_account);

	/** Store a Commodity object in the database
	 *
	 * @todo Verify that throwing behaviour is as documented.
	 *
	 * @throws sqloxx::SQLiteException if:\n
	 * 	SQLite statement preparation fails; or\n
	 * 	Insertion into database would cause the (auto-incrementing)
	 * 	primary key to overflow IdType; or\n
	 * 	Insertion fails (for reasons other than above).
	 *
	 * @param p_commodity the Commodity to be stored.
	 *
	 * @returns the integer primary key of the Account just stored.
	 * (This is an autoincrementing primary key.)
	 */
	IdType store(Commodity const& p_commodity);

	/**
	 * Returns true if and only if \c name is the name of an Account
	 * stored in the database.
	 */
	bool has_account_named(std::string const& p_name);

	/**
	 * Creates tables required for Phatbooks, and inserts rows
	 * into certain tables to provide application-level data where
	 * required. If the database already contains these tables,
	 * the function does nothing. This function should always be
	 * called after calling DatabaseConnection::open.
	 *
	 * @throws SQLiteException if unsuccesful.
	 */
	void setup();

private:
	bool setup_has_occurred();
	static std::string const s_setup_flag;

};  // PhatbooksDatabaseConnection



}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp

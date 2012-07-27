#ifndef GUARD_phatbooks_database_connection_hpp
#define GUARD_phatbooks_database_connection_hpp

#include "account.hpp"
#include "database_connection.hpp"

namespace phatbooks
{

/** Phatbooks-specific database connection class.
 * See API documentation for sqloxx::DatabaseConnection,
 * for parts of API inherited from sqloxx::DatabaseConnection.
 *
 * @todo In setup_tables, commodity_id can be null. This is a
 * temporary edit / hack. It is essential that this be fixed.
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
 */
class PhatbooksDatabaseConnection:
	public sqloxx::DatabaseConnection
{
public:
	PhatbooksDatabaseConnection();
	
	/** Store an Account object in the database
	 *
	 * @throws sqloxx::SQLiteException if:
	 * 	SQLite statement preparation fails.
	 */
	void store(Account const& p_account);

private:
	void setup_tables();	


};  // PhatbooksDatabaseConnection



}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp

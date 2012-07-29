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
	 * @throws sqloxx::SQLiteException if:
	 * 	SQLite statement preparation fails;
	 * 	insertion fails (for reasons other than above).
	 *
	 * @param p_account the account to be stored.
	 */
	void store(Account const& p_account);

	/** Store a Commodity object in the database
	 *
	 * @todo Verify that throwing behaviour is as documented.
	 *
	 * @throws sqloxx::SQLiteException if:
	 * 	SQLite statement preparation fails;
	 * 	insertion fails.
	 */
	void store(Commodity const& p_commodity);


private:
	void setup();	
	bool setup_has_occurred();
	static std::string const s_setup_flag;

};  // PhatbooksDatabaseConnection



}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp

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

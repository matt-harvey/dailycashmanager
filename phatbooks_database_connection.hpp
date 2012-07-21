#ifndef GUARD_phatbooks_database_connection_hpp
#define GUARD_phatbooks_database_connection_hpp

#include "database_connection.hpp"

namespace phatbooks
{

/** Phatbooks-specific database connection class.
 * See API documentation for sqloxx::DatabaseConnection,
 * for parts of API inherited from sqloxx::DatabaseConnection.
 */
class PhatbooksDatabaseConnection:
  public sqloxx::DatabaseConnection
{
public:
	PhatbooksDatabaseConnection();
private:
	void setup_tables();	


};  // PhatbooksDatabaseConnection



}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp

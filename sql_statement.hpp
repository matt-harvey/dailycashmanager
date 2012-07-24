#ifndef GUARD_sql_statement_hpp
#define GUARD_sql_statement_hpp

#include "database_connection.hpp"
#include <sqlite3.h>

namespace sqloxx
{

/**
 * Wrapper class for sqlite_stmt*.
 *
 * @todo Should this class be nested within DatabaseConnection? Or,
 * should I have all SQLStatement functions be private and then
 * have DatabaseConnection declare it a friend?
 */
class SQLStatement
{
public:

	SQLStatement();

	~SQLStatement();

	// Can this param be const&?
	void prepare(DatabaseConnection const&);

	// I need to duplicate this for various different types
	// that can be bound into SQLite statements.
	void bind(char const* parameter_name, char const* value);


private:
	sqlite3_stmt* m_statement;
	
	// Do not implement - to capture incorrect types.
	template <typename T>
	void
	bind(char const* parameter_name, T value);

	


};


}  // namespace sqloxx

#endif  // GUARD_sql_statement_hpp

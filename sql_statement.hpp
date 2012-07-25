#ifndef GUARD_sql_statement_hpp
#define GUARD_sql_statement_hpp

#include "database_connection.hpp"
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <sqlite3.h>
#include <string>

namespace sqloxx
{

/**
 * Wrapper class for sqlite_stmt*.
 *
 * @todo Should this class be nested within DatabaseConnection? Or,
 * should I have all SQLStatement functions be private and then
 * have DatabaseConnection declare it a friend?
 */
class SQLStatement:
	private boost::noncopyable
{
public:

	// Can dbconn be const&?
	SQLStatement(DatabaseConnection& dbconn, std::string const& str);

	~SQLStatement();

	// I need to duplicate this for various different types
	// that can be bound into SQLite statements.
	void bind(std::string const& parameter_name, int value);
	void bind(std::string const& parameter_name, boost::int64_t value);
	void bind(std::string const& parameter_name, std::string);

	// Execution of SQLite statements occurs by calling step function.
	// This enables rows to be extracted one-by-one. Values in columns
	// can then be extracted too. There needs to be a mechanism to
	// achieve this.
	void execute();


private:
	sqlite3_stmt* m_statement;
	
	// Do not implement - to capture incorrect types.
	template <typename T>
	void
	bind(char const* parameter_name, T value);

	


};


}  // namespace sqloxx

#endif  // GUARD_sql_statement_hpp

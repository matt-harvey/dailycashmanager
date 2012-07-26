#ifndef GUARD_sql_statement_hpp
#define GUARD_sql_statement_hpp

#include "database_connection.hpp"
#include <boost/noncopyable.hpp>
#include <sqlite3.h>
#include <string>

namespace sqloxx
{

/**
 * Wrapper class for sqlite_stmt*.
 *
 * @todo Should this class be nested within DatabaseConnection?
 */
class SQLStatement:
	private boost::noncopyable
{
public:

	SQLStatement(DatabaseConnection& dbconn, std::string const& str);

	~SQLStatement();

	// Wrapper around SQLite bind functions
	void bind(std::string const& parameter_name, double value);
	void bind(std::string const& parameter_name, int value);
	void bind(std::string const& parameter_name, std::string const& str);

	// Wraps sqlite3_step
	// Returns true as long as there are further steps to go.
	bool step();


private:
	sqlite3_stmt* m_statement;
	DatabaseConnection& m_database_connection;

	// Return index no. of named parameter in statement
	int parameter_index(std::string const& parameter_name) const;

	// Check code is SQLITE_OK and if not finalize statement and
	// throw SQLiteException.
	void check_ok(int err_code);

};


}  // namespace sqloxx

#endif  // GUARD_sql_statement_hpp

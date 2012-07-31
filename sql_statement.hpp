#ifndef GUARD_sql_statement_hpp
#define GUARD_sql_statement_hpp

#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <boost/noncopyable.hpp>




namespace sqloxx
{



class DatabaseConnection::SQLStatement:
	private boost::noncopyable
{
public:

	SQLStatement(DatabaseConnection& dbconn, std::string const& str);

	~SQLStatement();

	// Wrapper around SQLite bind functions
	void bind(std::string const& parameter_name, double value);
	void bind(std::string const& parameter_name, int value);
	void bind(std::string const& parameter_name, boost::int64_t value);
	void bind(std::string const& parameter_name, std::string const& str);

	/**
	 * Where a SQLStatement has a result set available,
	 * this function (template) can be used to extract the value at
	 * the \c indexth column of the current row (where \c index starts
	 * counting at 0).
	 *
	 * Currently the following types for T are supported:\n
	 *	\c boost::int64_t\n
	 *	int\n
	 *	double\n
	 *	std::string\n
	 *
	 * @param index is the column number (starting at 0) from which to
	 * read the value.
	 * 
	 * @throws SQLiteException if:\n
	 * 	the index is out of range; or\n
	 * 	the requested column contains type that is incompatible with T.
	 */
	template <typename T>
	T extract(int index);


	// Wraps sqlite3_step
	// Returns true as long as there are further steps to go.
	bool step();

	// For executing statements which are not expected to return a result
	// set. SQLiteException is thrown if there are one or more result rows.
	void quick_step();


private:
	sqlite3_stmt* m_statement;
	DatabaseConnection& m_database_connection;

	// Return index no. of named parameter in statement
	int parameter_index(std::string const& parameter_name) const;

	/**
	 * Checks whether a column is available for extraction at
	 * index \c index, of type \c value_type, and throws an
	 * exception if not.
	 *
	 * @param index Position of column (starts from zero) in result
	 * row.
	 * 
	 * @param value_type Should be a SQLite value type code, i.e. one of:\n
	 * 	SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, SQLITE_NULL.
	 *
	 * @throws SQLiteException if:\n
	 * 	There are no results available for extraction;\n
	 * 	\c index is out of range; or\n
	 * 	\c the value at position \c index is not of value type \c value_type.
	 */
	void check_column(int index, int value_type);

	// Check code is SQLITE_OK and if not finalize statement and
	// throw SQLiteException.
	void check_ok(int err_code);


};


// FUNCTION TEMPLATE DEFINITIONS


template <>
inline
int
DatabaseConnection::SQLStatement::extract<int>(int index)
{
	check_column(index, SQLITE_INTEGER);
	return sqlite3_column_int(m_statement, index);
}

template <>
inline
boost::int64_t
DatabaseConnection::SQLStatement::extract<boost::int64_t>(int index)
{
	check_column(index, SQLITE_INTEGER);
	return sqlite3_column_int64(m_statement, index);
}

template <>
inline
double
DatabaseConnection::SQLStatement::extract<double>(int index)
{
	check_column(index, SQLITE_FLOAT);
	return sqlite3_column_double(m_statement, index);
}

template <>
inline
std::string
DatabaseConnection::SQLStatement::extract<std::string>(int index)
{
	check_column(index, SQLITE_TEXT);
	const unsigned char* begin = sqlite3_column_text(m_statement, index);
	const unsigned char* end = begin;
	while (*end != '\0') ++end;
	return std::string(begin, end);
}



}  // namespace sqloxx

#endif  // GUARD_sql_statement_hpp

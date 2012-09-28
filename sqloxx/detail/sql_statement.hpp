#ifndef GUARD_sql_statement_hpp
#define GUARD_sql_statement_hpp

/** \file sql_statement.hpp
 *
 * \brief Header file pertaining to DatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 *
 * @todo Move sqloxx code to a separate library.
 *
 * @todo I am often calling sqlite3_finalize() before throwing
 * SQLiteException(). Is this necessary? Is this desirable? What
 * about if reset() is preferred by the caller?
 */


#include "sqloxx/sqloxx_exceptions.hpp"
#include <jewel/checked_arithmetic.hpp>
#include <sqlite3.h>
#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <cassert>
#include <limits>
#include <string>
#include <vector>



namespace sqloxx
{
namespace detail
{

// Forward declaration
class SQLiteDBConn;

/**
 * Wrapper class for sqlite_stmt*. This class is not intended to be
 * used except internally by the Sqloxx library. SQLStatement instances
 * are themselves encapsulated by SharedSQLStatement instances.
 *
 * @todo The constructor to create a SQLStatement should reject strings
 * containing semicolons, since compound statements are not handled by
 * step() properly. There should be some other class SQLMultiStatement or
 * something, which can then executed using a wrapper around sqlite3_exec.
 */
class SQLStatement:
	private boost::noncopyable
{
public:

	/**
	 * Creates an object encapsulating a SQL statement.
	 *
	 * @throws InvalidConnection if the database connection passed to
	 * \c dbconn is invalid.
	 *
	 * @throws SQLiteException or an exception derived therefrom, if
	 * the database connection is valid, but the statement could not
	 * be properly prepared by SQLite.
	 */
	SQLStatement(SQLiteDBConn& p_sqlite_dbconn, std::string const& str);

	~SQLStatement();

	/**
	 * Wrappers around SQLite bind functions.
	 *
	 * These throw \c SQLiteException, or an exception derived therefrom,
	 * if SQLite could not properly bind the statement.
	 */
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
	 * @throws ResultIndexOutOfRange if \c index is out of range.
	 *
	 * @throws ValueTypeException if the requested column contains a type that
	 * is incompatible with T.
	 */
	template <typename T>
	T extract(int index);

	/**
	 * Wraps sqlite3_step
	 * Returns true as long as there are further steps to go (i.e. result
	 * rows to examine).
	 *
	 * @throws SQLiteException or some exception deriving therefrom, if an
	 * error occurs. This function should almost never throw, but it is
	 * possible something will fail as the statement is being executed, in
	 * which the resulting SQLite error condition will trigger the
	 * corresponding exception class.
	 */
	bool step();

	/**
	 * Wraps sqlite3_step. Similar to \c step except that it throws an
	 * exception if a result row still remains after calling. That is,
	 * it is equivalent to calling:\n
	 * \c if (step()) throw UnexpectedResultRow("...");\n
	 *
	 * @throws UnexpectedResultRow if a result set is returned.
	 * 
	 * @throws SQLiteException or an exception derived therefrom if there
	 * is any other error in executing the statement.
	*/
	void step_last();

	/**
	 * Resets the statement, freeing bound parameters ready for
	 * subsequent re-binding and re-execution.
	 *
	 * Does not throw.
	 */
	void reset();

private:
	
	/**
	 * @parameter_name is the name of a column in the result set.
	 * 
	 * @throws NoMatchingColumnException if \c parameter_name does not
	 * name a column in the result set.
	 */
	int parameter_index(std::string const& column_name) const;

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
	 * @throws NoResultRowException if there are no results available for
	 * extraction.
	 *
	 * @throws ResultIndexOutOfRange if \c index is negative or is otherwise
	 * out of range.
	 *
	 * @throws ValueTypeException if the value at position \c index is not of
	 * value type \c value_type.
	 */
	void check_column(int index, int value_type);

	/**
	 * Check code is SQLITE_OK and if not finalize statement and
	 * throw SQLiteException or derivative corresponding to the SQLite
	 * error code.
	 *
	 * @throw If error code is SQLITE_DONE or SQLITE_ROWS then this
	 * throws SQLiteUnknownErrorCode. Improve this behaviour.
	 */
	void check_ok();

	sqlite3_stmt* m_statement;
	SQLiteDBConn& m_sqlite_dbconn;

};


// FUNCTION TEMPLATE DEFINITIONS

template <>
inline
int
SQLStatement::extract<int>(int index)
{
	check_column(index, SQLITE_INTEGER);
	return sqlite3_column_int(m_statement, index);
}

template <>
inline
boost::int64_t
SQLStatement::extract<boost::int64_t>(int index)
{
	check_column(index, SQLITE_INTEGER);
	return sqlite3_column_int64(m_statement, index);
}

template <>
inline
double
SQLStatement::extract<double>(int index)
{
	check_column(index, SQLITE_FLOAT);
	return sqlite3_column_double(m_statement, index);
}

template <>
inline
std::string
SQLStatement::extract<std::string>(int index)
{
	check_column(index, SQLITE_TEXT);
	const unsigned char* begin = sqlite3_column_text(m_statement, index);
	const unsigned char* end = begin;
	while (*end != '\0') ++end;
	return std::string(begin, end);
}



}  // namespace detail
}  // namespace sqloxx

#endif  // GUARD_sql_statement.hpp

#ifndef GUARD_database_connection_hpp
#define GUARD_database_connection_hpp

#include "sqloxx_exceptions.hpp"
#include <jewel/checked_arithmetic.hpp>
#include <jewel/debug_log.hpp>
#include <sqlite3.h>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <cassert>
#include <limits>
#include <string>
#include <vector>

/**
 *
 * @todo Move sqloxx code to a separate library.
 * 
 * @todo Make the DatabaseConnection class provide for a means
 * to check whether the particular file being connected to is
 * the right kind of file for the application that is using
 * the DatabaseConnection class. At the moment it just checks whether
 * the file exists. This could involve overriding some method or other.
 *
 * @todo Use boost::filesystem::path to make filepath passed to
 * activate_database more portable.
 *
 * @todo Consider supplying public member function to close any
 * database connections and shut down SQLite3. Current this is done
 * in the destructor, but this can't throw.
 *
 * @todo DatabaseConnection::is_valid should probably do more than
 * just check whether m_connection exists. It should probably also
 * at least check SQLite error status.
 *
 * @todo I am often calling sqlite3_finalize() before throwing
 * SQLiteException(). Is this necessary? Is this desirable? What
 * about if reset() is preferred by the caller?
 */




namespace sqloxx
{



/**
 * Class to manage connection to SQLite3 database and execute
 * SQL on that database. 
 *
 * This class can be derived from and certain member functions
 * overridden to create application-specific database code.
 */
class DatabaseConnection:
	private boost::noncopyable
{
public:


	/**
	 * Initializes SQLite3 and creates a database connection
	 * initially set to null.
	 *
	 * @throws SQLiteException if SQLite3 initialization fails,
	 * or if database connection cannot be opened.
	 */
	DatabaseConnection();

	/**
	 * Closes any open SQLite3 database connection, and also
	 * shuts down SQLite3.
	 *
	 * Does not throw. If SQLite3 connection closure or shutdown fails,
	 * the application is aborted with a diagnostic message written to
	 * std::clog.
	 */
	virtual ~DatabaseConnection();

	/**
	 * Returns \c true iff the DatabaseConnection is connected to a 
	 * database.
	 */
	virtual bool is_valid();

	/**
	 * Points the database connection to a specific file
	 * given by \c filename. If the file
	 * does not already exist it is created.
	 *
	 * @param filename file to connect to
	 *
	 * @throws SQLiteException if SQLite3
	 * if database connection cannot be opened to the specified file.
	 */
	void open(char const* filename);	


protected:

	/**
	 * Wrapper class for sqlite_stmt*.
	 *
	 * @todo The constructor to create a SQLStatement should reject strings
	 * containing semicolons, since compound statements are not handled by
	 * step() properly. There should be some other class SQLMultiStatement or
	 * something, which can then executed using a wrapper around sqlite3_exec.
	 */
	class SQLStatement;


	/**
	 * Throws a SQLiteException with the current sqlite3_errmsg passed
	 * to the constructor of the exception.
	 *
	 * This is essentially to save typing.
	 *
	 * @throws SQLiteException whenever called
	 */
	void throw_sqlite_exception();

	/**
	 * Executes a string on the database connection.
	 * This should be used only where the developer has complete
	 * control of the string being passed, to prevent SQL injection
	 * attacks. Generally, the functions provided by SQLStatement should
	 * be the preferred means for building and executing SQL statements.
	 */
	void execute_sql(std::string const& str);




	/**
	 * Given the name of a table in the connected database, assuming that
	 * table has a single-column primary key, and assuming that column is
	 * an autoincrementing primary key, this function will return the next
	 * highest key value (the value 1 greater than the highest primary key
	 * so far in the table). This will be the next key assigned by SQLite
	 * for an ordinary insertion into the table (assuming the default
	 * behaviour of SQLite in this regard has not been altered in some way).
	 * This function does NOT check whether the primary
	 * key is in fact autoincrementing. However, it DOES check whether the
	 * table has a single-column primary key, and will throw an exception if
	 * it doesn't. In addition, it will throw an exception if the next highest
	 * key would exceed the maximum value for KeyType.
	 *
	 * Assumes keys start from 1.
	 *
	 * KeyType should be an integral type, and should also be a type
	 * supported by SQLStatement::extract. If not, behaviour is \e undefined,
	 * although it is expected that compilation will fail where a KeyType
	 * that is not accepted by SQLStatement::extract is provided.
	 * 
	 * It is the caller's responsibility to ensure that KeyType is large
	 * enough to accommodate the values that are \e already in the
	 * primary key of the table - otherwise behaviour is undefined.
	 *
	 * This function should not be used if \c table_name is an untrusted
	 * string.
	 *
	 * @todo Make this more efficient.
	 *
	 * @param table_name The name of the table. 
	 *
	 * @returns the next highest primary key for the table, assuming it has
	 * a single-column primary key. Note if there are gaps in the numbering
	 * these are ignored. The returned value is always one greater than the
	 * currently greatest value for the key (but see exceptions).
	 *
	 * @throws SQLiteException if:\n
	 *   The table does not have a primary key;\n
	 *   The table has a compound i.e. multi-column primary key; or\n
	 *   The greatest primary key value already in the table is the maximum
	 *   value for KeyType, so that another row could not be inserted without
	 *   overflow.
	 */
	template<typename KeyType>
	KeyType next_auto_key(std::string const& table_name);	


	/**
	 * To find primary key of a table.
	 *
	 * Note, this function should not be used where \c table_name is
	 * an untrusted string.
	 *
	 * @todo To speed execution, assuming the return value for a given
	 * \c table_name never changes (is this a safe assumption?), the
	 * return values could be cached either in a
	 * map< string, vector<string> >, or by way of templatizing this
	 * function with \c table_name as a (non-type) template parameter,
	 * and the return value stored as a static variable inside the
	 * function.
	 *
	 * @returns a vector of the names of all the columns making up the
	 * primary key of the table named \c table_name. An empty vector
	 * is returned if there is no primary key.
	 *
	 * @throws SQLiteException in case of invalid table name or other
	 * SQL execution error.
	 *
	 * @param table_name name of table
	 */
	std::vector<std::string> primary_key(std::string const& table_name);

	
private:

	/**
	 * A connection to a SQLite3 database file.
	 *
	 * (Note this is a raw pointer not a smart pointer
	 * to facilitate more straightforward interaction with the SQLite
	 * C API.)
	 */
	sqlite3* m_connection;

};




// DEFINITIONS OF NESTED CLASSES


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
	// set. SQLiteException is thrown if there is a result set.
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

template<typename KeyType>
inline
KeyType
DatabaseConnection::next_auto_key(std::string const& table_name)
{
	std::vector<std::string> const pk = primary_key(table_name);
	switch (pk.size())
	{
	case 0:
		throw SQLiteException("Table has no primary key.");
		assert (false);  // Never executes
	case 1:
		break;
	default:
		assert (pk.size() > 1);
		throw SQLiteException("Table has a multi-column primary key.");
		assert (false);  // Never executes;
	}
	assert (pk.size() == 1);
	std::string const key_name = pk[0];
	
	// Note we can't use binding to put key_name into statement here as this
	// will result in binding a string into the max function when we need
	// an integer. At least we know that if table_name is safe, so is
	// key_name (since it must be a valid column name).
	
	// First count rows to see if there are any
	SQLStatement row_counter
	(	*this,
		"select count(" + key_name + ") from " + table_name
	);
	bool check = row_counter.step();
	assert (check);
	KeyType const row_count = row_counter.extract<KeyType>(0);
	check = row_counter.step();
	assert (!check);
	if (row_count == 0)
	{
		return 1;
	}

	assert (row_count > 0);
	
	// Then find the max - we do this separately as there might
	// be gaps in the numbering.
	SQLStatement max_finder
	(	*this, 
		"select max(" + key_name + ") from " + table_name
	);
	check = max_finder.step();
	if (!check)
	{
		throw SQLiteException("Error finding max of primary key.");
	}
	assert (check);
	KeyType const max_key = max_finder.extract<KeyType>(0);		
	check = max_finder.step();
	// By the nature of the SQL max function, there must have been no more
	// than one result row.
	assert (!check);
	if (max_key == std::numeric_limits<KeyType>::max())
	{
		throw SQLiteException
		(	"Key cannot be safely incremented with given type."
		);
	}
	return max_key + 1;
	
}


}  // namespace sqloxx

#endif  // GUARD_database_connection_hpp

#ifndef GUARD_next_auto_key
#define GUARD_next_auto_key

#include "sql_statement.hpp"
#include "sqloxx_exceptions.hpp"
#include <limits>
#include <string>

namespace sqloxx
{


/**
 * Given the name of a table in the connected database \e dbc,
 * assuming that
 * table has a single-column primary key, and assuming that column is
 * an autoincrementing primary key, this function will return the next
 * highest key value (the value 1 greater than the highest primary key
 * so far in the table). This will be the next key assigned by SQLite
 * for an ordinary insertion into the table (assuming the default
 * behaviour of SQLite in this regard has not been altered in some way).
 * This function does NOT check whether the primary
 * key is in fact autoincrementing, or even whether there is a primary
 * key, or even whether the table with this name exists. In the event
 * the next primary key can't be found, a
 * value of \c KeyType(1) is simply returned. In other words, it is
 * the caller's responsibility to make sure that table_name does in
 * fact correspond to a table with an autoincrementing integer
 * primary key.
 *
 * Assumes keys start from 1.
 *
 * Important constraints: KeyType should be an integral type, and should
 * also be a type supported by SQLStatement::extract. If not, behaviour
 * is \e undefined, although it is expected that compilation will fail
 * where a KeyType that is not accepted by SQLStatement::extract is
 * provided.
 * 
 * It is the caller's responsibility to ensure that KeyType is large
 * enough to accommodate the values that are \e already in the
 * primary key of the table - otherwise behaviour is undefined.
 *
 * This function should not be used if \c table_name is an untrusted
 * string.
 *
 * @param table_name The name of the table
 *
 * @returns the next highest primary key for the table, assuming it has
 * a single-column primary key. Note if there are gaps in the numbering
 * these are ignored. The returned value is always one greater than the
 * currently greatest value for the key (but see exceptions).
 * 
 * @throws sqloxx::TableSizeException if the greatest primary key value 
 * already in the table is the maximum value for \e KeyType, so that
 * another row could not be inserted without overflow.
 *
 * @throws sqloxx::DatabaseException, or a derivative therefrom, may
 * be thrown if there is some other
 * error finding the next primary key value. This should not occur except
 * in the case of a corrupt database, or a memory allocation error
 * (extremely unlikely), or the database connection being invalid
 * (including because not yet connected to a database file).
 *
 * @throws sqloxx::InvalidConnection if database connection is invalid.
 *
 * Exception safety: <em>strong guarantee</em>, provided client adheres to
 * the constraints described above.
 */
template <typename Connection, typename KeyType>
KeyType next_auto_key(Connection& dbc, std::string const& table_name);	


// Definition
template <typename Connection, typename KeyType>
KeyType
next_auto_key(Connection& dbc, std::string const& table_name)
{
	try
	{
		SQLStatement statement
		(	dbc,
			"select seq from sqlite_sequence where name = :p"
		);
		statement.bind(":p", table_name);
		if (!statement.step())
		{
			return 1;
		}
		KeyType const max_key = statement.extract<KeyType>(0);
		if (max_key == std::numeric_limits<KeyType>::max())
		{
			throw TableSizeException
			(	"Key cannot be safely incremented with given type."
			);
		}
		return max_key + 1;
	}
	catch (SQLiteError&)
	{
		// Catches case where there is no sqlite_sequence table
		SQLStatement sequence_finder
		(	dbc,
			"select name from sqlite_master where type = 'table' and "
			"name = 'sqlite_sequence';"
		);
		if (!sequence_finder.step())
		{
			return 1;
		}
		throw;
	}
}



}  // namespace sqloxx


#endif  // GUARD_next_auto_key

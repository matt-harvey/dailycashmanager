#ifndef GUARD_database_connection_hpp
#define GUARD_database_connection_hpp

/** \file database_connection.hpp
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
 * @todo Make the DatabaseConnection class provide for a means
 * to check whether the particular file being connected to is
 * the right kind of file for the application that is using
 * the DatabaseConnection class. At the moment it just checks whether
 * the file exists. This could involve overriding some method or other.
 *
 * @todo There should probably be a close method. Even though the
 * connection is closed by the destructor, there should probably
 * be a way of closing it independently, so that it can be connected
 * elsewhere.
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


#include "sqloxx_exceptions.hpp"
#include "sql_statement.hpp"
#include "storage_manager.hpp"
#include <jewel/checked_arithmetic.hpp>
#include <jewel/debug_log.hpp>
#include <sqlite3.h>
#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <cassert>
#include <limits>
#include <string>
#include <vector>




/**
 * Namespace for housing all Sqloxx code. Sqloxx is intended as a
 * thin wrapper around SQLite, to facilitate using SQLite from C++,
 * to facilitate managing the resources required by SQLite using RAII,
 * and to reduce the verbosity required in client code wishing to
 * using the SQLite library. Sqloxx will not necessarily provide
 * much in the way of objected-relational mapping. The intention is
 * that the C++ API of Sqloxx will largely mirror the C API of
 * SQLite, so that Sqloxx could be used easily by anyone who is
 * familiar with SQLite (and with C++).
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
	friend class SQLStatement;
public:


	/**
	 * Initializes SQLite3 and creates a database connection
	 * initially set to null.
	 *
	 * @throws SQLiteInitializationError if initialization fails
	 * for any reason.
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
	 * database. Does not throw.
	 */
	virtual bool is_valid() const;

	/**
	 * Points the database connection to a specific file
	 * given by \c filename. If the file
	 * does not already exist it is created. Note the SQLite pragma
	 * foreign_keys is always executed immediately the file is opened, to
	 * enable foreign key constraints.
	 *
	 * @todo This should be made to support Unicode filepaths, which
	 * apparently are used on Windows.
	 *
	 * @todo It appears that boost::filesystem::path::string() produces
	 * a GENERIC string (safely passable to SQLite database connection
	 * opening function) in Boost Version 1.42; but that in Version 1.46
	 * this produced a NATIVE string! Currently this relies on the
	 * behaviour in version 1.42. I should use a macro or something to
	 * make it portable between versions of Boost.
	 *
	 * @param filepath File to connect to. The is in the form of a
	 * \c boost::filesystem::path to facilitate portability.
	 *
	 * @todo Do a full portability test to Windows, especially for cases
	 * involving escape characters and such.
	 *
	 * @throws sqloxx::InvalidFilename if filename is an empty string.
	 *
	 * @throws sqloxx::MultipleConnectionException if already connected to a
	 * database (be it this or another database).
	 *
	 * @throws SQLiteException or an exception derived therefrom (likely, but
	 * not guaranteed, to be SQLiteCantOpen) if for some other reason the
	 * connection cannot be opened.
	 */
	void open(boost::filesystem::path const& filepath);	

	/**
	 * If StorageManager<T> has been specialised for T, then
	 * this template causes a table to be created for the storage
	 * of objects of type T, and possibly some other code executed according
	 * to what is provided for in the StorageManager<T> setup_tables() method.
	 *
	 * @todo Determine and document throwing behavior.
	 */
	template <typename T>
	void setup_tables();


	/**
	 * If StorageManager<T> has been specialised for T, then
	 * this template provides a function for an instance \c obj of
	 * \c T to be stored in the database.
	 * 
	 * See documentation for StorageManager class template for more
	 * information on what is required to be implemented by the class
	 * template specialisation.
	 *
	 * @todo Determine and document throwing behaviour.
	 */
	template <typename T>
	void save(T const& obj);

	/**
	 * If StorageManager<T> has been specialised for T, then
	 * this template provides a function for an instance of \c T to be loaded
	 * and returned by the function by looking into the database using \c key
	 * as a key.
	 *
	 * See documentation for StorageManager class template for more
	 * information on what is required to be implemented by the class template
	 * specialisation.
	 *
	 * @todo Determine and document throwing behaviour.
	 */
	template <typename T>
	T load(typename T::Key const& key);



	/**
	 * Executes a string on the database connection.
	 * This should be used only where the developer has complete
	 * control of the string being passed, to prevent SQL injection
	 * attacks. Generally, the functions provided by SQLStatement should
	 * be the preferred means for building and executing SQL statements.
	 *
	 * @throws SQLiteException or some exception inheriting thereof, whenever
	 * there is any kind of error executing the statement.
	 */
	void execute_sql(std::string const& str);



	/**
	 * If the database connection is in an error state recognized by SQLite,
	 * this throws a \c SQLiteException with the current sqlite3_errmsg passed
	 * to the constructor of the exception. The exact exception thrown
	 * corresponds to the current SQLite error code for the connection. Any
	 * thrown exception will be an instance of class that is, or extends,
	 * \c SQLiteException.
	 *
	 * @todo If error code is SQLITE_DONE or SQLITE_ROWS, this throws
	 * SQLiteUnknownErrorCode. Improve this behaviour.
	 */
	void check_ok();

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
	 * @todo LOW PRIORITY Find a way either to make the body of this function
	 * template briefer, or to get it out of the header file.
	 *
	 * @throws sqloxx::NoPrimaryKeyException if the table does not have a
	 * primary key.
	 *
	 * @throws sqloxx::CompoundPrimaryKeyException if the table has a
	 * compound primary
	 * key.
	 *
	 * @throws sqloxx::TableSizeException if the greatest primary key value 
	 * already in the table is the maximum value for \c KeyType, so that
	 * another row could not be inserted without overflow.
	 *
	 * @throws sqloxx::DatabaseException if there is some other error finding
	 * the next primary key value.
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
	 * @todo Determine and document throwing behaviour.
	 *
	 * @throws InvalidConnection if database connection does not exist or
	 * is otherwise invalid.
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




// FUNCTION TEMPLATE DEFINITIONS

template <typename T>
inline
void
DatabaseConnection::save(T const& obj)
{
	StorageManager<T>::save(obj, *this);
	return;
}

template <typename T>
inline
T
DatabaseConnection::load(typename T::Key const& key)
{
	return StorageManager<T>::load(key, *this);
}

template <typename T>
inline
void
DatabaseConnection::setup_tables()
{
	StorageManager<T>::setup_tables(*this);
	return;
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
		throw NoPrimaryKeyException("Table has no primary key.");
		assert (false);  // Never executes
	case 1:
		break;
	default:
		assert (pk.size() > 1);
		throw CompoundPrimaryKeyException
		(	"Table has a multi-column primary key."
		);
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
		throw DatabaseException("Error finding max of primary key.");
	}
	assert (check);
	KeyType const max_key = max_finder.extract<KeyType>(0);		
	check = max_finder.step();
	// By the nature of the SQL max function, there must have been no more
	// than one result row.
	assert (!check);
	if (max_key == std::numeric_limits<KeyType>::max())
	{
		throw TableSizeException
		(	"Key cannot be safely incremented with given type."
		);
	}
	return max_key + 1;
	
}


}  // namespace sqloxx

#endif  // GUARD_database_connection_hpp

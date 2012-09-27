#ifndef database_connection_hpp
#define database_connection_hpp

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include "shared_sql_statement.hpp"
#include "sqlite_dbconn.hpp"

namespace sqloxx
{

class SQLStatement;
class SQLStatementManager;


/**
 * @todo HIGH PRIORITY The new architecture, for enabling the caching and
 * reuse of SQLiteStatement instances, needs some house-keeping. In
 * particular, there are classes (like SQLiteDBConn, SQLStatement and
 * SQLStatementManager) that are not intended for use outside of sqloxx.
 * Access to these should be restricted accordingly (though perhaps
 * there is a case for making SQLStatement potentially publically
 * accessible, as there may be cases where clients don't need the
 * overhead associated with caching). Furthermore, the public-facing
 * API of sqloxx needs to be thoroughly documented; while the
 * inner workings need to be documented too, but with an eye
 * to clarifying the internals workings of sqloxx, rather than
 * providing an interface to clients.
 *
 * @todo Have begin_transaction and end_transaction use SharedSQLStatement
 * instead of execute_sql. This is likely to speed execution.
 */
class DatabaseConnection:
	private boost::noncopyable
{
	friend class SharedSQLStatement;
public:
	
	/**
	 * @todo Provide a way for cache capacity to be set from
	 * contructor.
	 */
	DatabaseConnection();

	virtual ~DatabaseConnection();

	virtual bool is_valid() const;

	void open(boost::filesystem::path const& filepath);

	void execute_sql(std::string const& str);

	void check_ok();

	void setup_boolean_table();

		
	/**
	 * To find primary key of a table.
	 *
	 * Note, this function should not be used where \c table_name is
	 * an untrusted string.
	 *
	 * @todo To speed execution, assuming the return value for a given
	 * \c table_name never changes, the return values could be cached in a
	 * map< string, vector<string> > inside the SQLiteDBConn
	 * instance.
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
	 *
	 * @todo Speed this up by having it consult the sqlite_sequence table
	 * where possible, rather than taking the maximum.
	 */
	template <typename KeyType>
	KeyType next_auto_key(std::string const& table_name);	

	/**
	 * Begins a transaction. Transactions may be nested. Only the
	 * outermost call to begin_transaction causes the "begin transaction"
	 * SQL command to be executed.
	 *
	 * Note this may fail silently in the unlikely event that the number of
	 * nested transactions exceeds MAX_INT.
	 */
	void begin_transaction();

	/**
	 * Ends a transaction. Transactions may be nested. Only the outermost
	 * call to end_transaction causes the "end transaction" SQL command
	 * to be executed.
	 *
	 * @throws TransactionNestingException in the event that there are
	 * more calls to end_transaction than there have been to
	 * begin_transaction.
	 */
	void end_transaction();


private:

	/**
	 * @returns a shared pointer to a SQLStatement. This will	
	 * either point to an existing SQLStatement that is cached within
	 * the SQLiteDBConn (if a SQLStatement with \c
	 * statement_text has already been created on this SQLiteDBConn), or
	 * will be a pointer to a newly created and new cached SQLStatement (if a 
	 * SQLStatement with \c statement_text has not yet been created on this
	 * SQLiteDBConn).
	 *
	 * This function is only intended to be called by the
	 * constructor of SharedSQLStatement. It should not be called elsewhere.
	 */
	boost::shared_ptr<SQLStatement> provide_sql_statement
	(	std::string const& statement_text
	);

	boost::shared_ptr<SQLiteDBConn> m_sqlite_dbconn;

	boost::shared_ptr<SQLStatementManager> m_sql_statement_manager;

	void unchecked_end_transaction();

	int m_transaction_nesting_level;
	
};


// FUNCTION TEMPLATE DEFINITIONS


template <typename KeyType>
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
	SharedSQLStatement row_counter
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
	SharedSQLStatement max_finder
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

inline
void
DatabaseConnection::unchecked_end_transaction()
{
	SharedSQLStatement statement
	(	*this,
		"end"
	);
	statement.step();
	return;
}


}  // namespace sqloxx

#endif  // database_connection_hpp

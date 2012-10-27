#ifndef database_connection_hpp
#define database_connection_hpp

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <set>
#include <string>
#include "shared_sql_statement.hpp"
#include "detail/sqlite_dbconn.hpp"


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
 *
 * @todo HIGH PRIORITY The API docs sometimes assume throw_on_failure will
 * only ever throw a derivative of SQLiteException; however InvalidConnection
 * is not a derivative of SQLiteException. Fix this.
 */
namespace sqloxx
{


// Forward declaration
namespace detail
{
	class SQLStatement;
}  // namespace detail



/**
 * @class DatabaseConnection
 *
 * Represents a SQLite3 database connection. Class can be extended to provide
 * representations of connections to application-specific databases.
 */
class DatabaseConnection:
	private boost::noncopyable
{
public:

	typedef
		boost::unordered_map
		<	std::string, boost::shared_ptr<detail::SQLStatement>
		>
		StatementCache;
	
	/**
	 * Initializes SQLite3 and creates a database connection initially
	 * set to null, i.e. not connected to any file.
	 *
	 * @param p_cache_capacity indicates the number of SQLStatements to
	 * be stored in a cache for reuse (via the class SharedSQLStatement)
	 * by the DatabaseConnection instance.
	 *
	 * Initializes SQLite3 and creates a database connection initially
	 * set to null, i.e. not connected to any file.
	 *
	 * @throws SQLiteInitializationError if initialization fails
	 * for any reason.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	explicit
	DatabaseConnection(StatementCache::size_type p_cache_capacity = 300);

	/**
	 * Exception safety: <em>nothrow guarantee<em>. (Of course, the exception
	 * safety of derived classes will depend on their own destructors.)
	 */
	virtual ~DatabaseConnection();

	/**
	 * @returns true if and only if there is a connection to a valid
	 * database.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
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
	 * this produces a NATIVE string! Currently this function relies on the
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
	 *
	 * Exception safety: appears to offer the <em>basic guarantee</em>,
	 * <em>however</em> this has not been properly tested.
	 */
	void open(boost::filesystem::path const& filepath);

	/**
	 * Executes a string as an SQL command on the database connection.
	 * This should be used only where the developer has complete
	 * control of the string being passed, to prevent SQL injection
	 * attacks. Generally, the functions provided by SQLStatement should
	 * be the preferred means for building and executing SQL statements.
	 *
	 * @throws DatabaseException or some exception inheriting thereof,
	 * whenever
	 * there is any kind of error executing the statement.
	 * 
	 * @throws InvalidConnection if the database connection is invalid.
	 * 
	 * Exception safety: <em>basic guarantee</em>. (Possibly also offers
	 * strong guarantee, but not certain.)
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
	 * @param table_name The name of the table
	 *
	 * @returns the next highest primary key for the table, assuming it has
	 * a single-column primary key. Note if there are gaps in the numbering
	 * these are ignored. The returned value is always one greater than the
	 * currently greatest value for the key (but see exceptions).
	 * 
	 * @throws sqloxx::TableSizeException if the greatest primary key value 
	 * already in the table is the maximum value for \c KeyType, so that
	 * another row could not be inserted without overflow.
	 *
	 * @throws sqloxx::DatabaseException, or a derivative therefrom, may
	 * be thrown if there is some other
	 * error finding the next primary key value. This should not occur except
	 * in the case of a corrupt database, or a memory allocation error
	 * (extremely unlikely), or the database connection being invalid
	 * (including because not yet connected to a database file).
	 */
	template <typename KeyType>
	KeyType next_auto_key(std::string const& table_name);	


	/**
	 * Creates table containing integers representing boolean values.
	 * This might be used to provide foreign key constraints for other
	 * tables where we wish a particular column to have boolean values
	 * only.
	 *
	 * The table is called "booleans" and has one column, an integer
	 * primary key field with the heading "representation". There are
	 * two rows, one with 0 in the "representation" column, representing
	 * \e false, and the other with 1, representing \e true.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	void setup_boolean_table();

	/**
	 * Returns the maximum level of transaction nesting that can be handled
	 * by the DatabaseConnection class.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	static int max_nesting();

	/**
	 * Begins a transaction. Transactions may be nested. Only the
	 * outermost call to begin_transaction causes the "begin transaction"
	 * SQL command to be executed.
	 *
	 * SQL transactions should be controlled either solely through the
	 * methods begin_transaction and end_transaction, \e or solely through
	 * the direct execution of SQL statement strings "begin transaction" and
	 * "end transaction". Mixing the two will result in undefined behaviour.
	 *  
	 * @throws TransactionNestingException in the event that the maximum
	 * level of nesting has been reached. The maximum level of nesting is
	 * equal to the value returned by max_nesting().
	 */
	void begin_transaction();

	/**
	 * Ends a transaction. Transactions may be nested. Only the outermost
	 * call to end_transaction causes the "end transaction" SQL command
	 * to be executed.
	 *
	 * See documentation of begin_transaction also.
	 *
	 * @throws TransactionNestingException in the event that there are
	 * more calls to end_transaction than there have been to
	 * begin_transaction.
	 */
	void end_transaction();

	/**
	 * @returns a shared pointer to a SQLStatement. This will	
	 * either point to an existing SQLStatement that is cached within
	 * the DatabaseConnection (if a SQLStatement with \c
	 * statement_text has already been created on this DatabaseConnection and
	 * is not being used elsewhere), or
	 * will be a pointer to a newly created and new cached SQLStatement (if a 
	 * SQLStatement with \c statement_text has not yet been created on this
	 * DatabaseConnection, or it has been created but is being used
	 * elsewhere).
	 *
	 * This function is only intended to be called by the
	 * constructor of SharedSQLStatement. It should not be called elsewhere.
	 */
	boost::shared_ptr<detail::SQLStatement> provide_sql_statement
	(	std::string const& statement_text
	);

private:

	void unchecked_end_transaction();

	boost::shared_ptr<detail::SQLiteDBConn> m_sqlite_dbconn;
	int m_transaction_nesting_level;
	StatementCache m_statement_cache;
	StatementCache::size_type m_cache_capacity;
	static int const s_max_nesting;
};


// FUNCTION TEMPLATE DEFINITIONS AND INLINE FUNCTIONS

inline
bool
DatabaseConnection::is_valid() const
{
	return m_sqlite_dbconn->is_valid();
}


inline
void
DatabaseConnection::open(boost::filesystem::path const& filepath)
{
	m_sqlite_dbconn->open(filepath);
	return;
}


inline
void
DatabaseConnection::execute_sql(std::string const& str)
{
	m_sqlite_dbconn->execute_sql(str);
	return;
}


template <typename KeyType>
inline
KeyType
DatabaseConnection::next_auto_key(std::string const& table_name)
{
	
	try
	{
		SharedSQLStatement statement
		(	*this,
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
		SharedSQLStatement sequence_finder
		(	*this,
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


inline
int
DatabaseConnection::max_nesting()
{
	return s_max_nesting;
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

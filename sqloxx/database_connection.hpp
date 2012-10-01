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
	 */
	explicit
	DatabaseConnection(StatementCache::size_type p_cache_capacity = 300);

	virtual ~DatabaseConnection();

	/**
	 * @returns true if and only if there is a connection to a valid
	 * database AND detail_id_valid() also returns true. By
	 * default, detail_is_valid() always returns true; however it may
	 * be redefined in derived classes to provide additional checks.
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
	 */
	void open(boost::filesystem::path const& filepath);

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
	 * To find primary key of a table.
	 *
	 * Note, this function should not be used where \c table_name is
	 * an untrusted string.
	 *
	 * @todo To speed execution, assuming the return value for a given
	 * \c table_name never changes, the return values could be cached in a
	 * map< string, set<string> > inside the detail::SQLiteDBConn
	 * instance.
	 *
	 * Populates \e result with the names of all the columns making up the
	 * primary key of the table named \c table_name. Note the names are
	 * simply inserted into the set. If there is no primary key then no
	 * names will be inserted. If \e result contains one or more elements
	 * prior to being passed to the function, these will \e not be deleted
	 * from \e result by this function, but the primary key names will simply
	 * be inserted alongside the existing elements.
	 *
	 * Note that as the result is stored inside a std::set, the elements of
	 * the set will be ordered according to the ordering behaviour of the set
	 * (which by default will be alphabetical order), rather than according to
	 * their order in the table schema.
	 *
	 * @todo Determine, document and test throwing behaviour.
	 *
	 * @throws InvalidConnection if database connection does not exist or
	 * is otherwise invalid.
	 *
	 * @param table_name name of table
	 */
	void find_primary_key
	(	std::set<std::string>& result,
		std::string const& table_name
	);

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
	 * key. In the event the next primary key can't be found, a
	 * value of \c KeyType(1) is simply returned. In other words, it is
	 * the caller's responsibility to make sure that table_name does in
	 * fact correspond to a table with an autoincrementing integer
	 * primary key. Note however that if the database contains \e no
	 * tables with autoincrementing primary keys, then instead of 1
	 * being returned, SQLiteException (or a derivative therefrom)
	 * will be thrown.
	 * 
	 * @todo Determine what derivative of SQLiteException is thrown in the
	 * case just described.
	 * 
	 * @todo Make the behaviour of this function in edge cases more
	 * consistent than what is described above.
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
	 * @throws sqloxx::SQLiteException if the there are \e no tables
	 * in the database with an autoincrementing primary key.
	 *
	 * @throws sqloxx::DatabaseException may be thrown if there is some other
	 * error finding the next primary key value.
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
	 */
	void setup_boolean_table();

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
	 * Note this may fail silently in the unlikely event that the number of
	 * nested transactions exceeds MAX_INT.
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

	/**
	 * By default this always returns true. However it may be overriden
	 * in derived classes as required.
	 */
	virtual bool detail_is_valid() const;

	boost::shared_ptr<detail::SQLiteDBConn> m_sqlite_dbconn;
	int m_transaction_nesting_level;
	StatementCache m_statement_cache;
	StatementCache::size_type m_cache_capacity;
};


// FUNCTION TEMPLATE DEFINITIONS AND INLINE FUNCTIONS

inline
bool
DatabaseConnection::is_valid() const
{
	return m_sqlite_dbconn->is_valid() && detail_is_valid();
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


inline
void
DatabaseConnection::check_ok()
{
	m_sqlite_dbconn->check_ok();
	return;
}


inline
bool
DatabaseConnection::detail_is_valid() const
{
	return true;
}


template <typename KeyType>
inline
KeyType
DatabaseConnection::next_auto_key(std::string const& table_name)
{
	SharedSQLStatement statement
	(	*this,
		"select seq from sqlite_sequence where name = :p"
	);
	statement.bind(":p", table_name);
	bool check = statement.step();
	if (!check)
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

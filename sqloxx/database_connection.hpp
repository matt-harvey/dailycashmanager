#ifndef database_connection_hpp
#define database_connection_hpp

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <set>
#include <string>
#include "handle.hpp"
#include "identity_map.hpp"
#include "sqloxx_exceptions.hpp"


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


// Forward declarations

namespace detail
{
	class SQLiteDBConn;
	class SQLStatement;
}  // namespace detail


/**
 * Convenience function to return a Handle<T> given a database connection
 * of type Connection and an id of type T::Id.
 *
 * @todo Document and test.
 */
template <typename T, typename Connection>
Handle<T>
get_handle(Connection& dbc, typename T::Id id);

/**
 * Convenience function to return a Handle<T> to a new, id-less business
 * object of type T, given a database connection of type Connection.
 *
 * @todo Document and test.
 */
template <typename T, typename Connection>
Handle<T>
get_handle(Connection& dbc);




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
	 * <em>however</em> this has not been properly tested. This wraps
	 * a SQLite function for which the error-safety is not clear.
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
	 * @throws InvalidConnection is the database connection is invalid (e.g.
	 * not connected to a file).
	 *
	 * @throws SQLiteException, or an exception inherited therefrom, if there
	 * is some other error setting up the table (should be rare). For example,
	 * if the table has been set up already.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	void setup_boolean_table();

	/**
	 * @returns maximum level of transaction nesting.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	static int max_nesting();

	/**
	 * Controls access to DatabaseConnection::provide_sql_statement,
	 * deliberately limiting this access to the class SharedSQLStatement.
	 */
	class StatementAttorney
	{
	public:
		friend class SharedSQLStatement;
	private:
		static boost::shared_ptr<detail::SQLStatement>
			provide_sql_statement
			(	DatabaseConnection& p_database_connection,
				std::string const& p_statement_text
			);
	};

	friend class StatementAttorney;

	/**
	 * Controls access to the database transaction control facilities
	 * of DatabaseConnection, deliberately
	 * limiting this access to the class DatabaseTransaction.
	 */
	class TransactionAttorney
	{
	public:
		friend class DatabaseTransaction;
	private:
		static void begin_transaction
		(	DatabaseConnection& p_database_connection
		);
		static void end_transaction
		(	DatabaseConnection& p_database_connection
		);
		static void cancel_transaction
		(	DatabaseConnection& p_database_connection
		);
	};

	friend class TransactionAttorney;

private:

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
	 * 
	 * @throws InvalidConnection if p_database_connection is an invalid
	 * connection.
	 *
	 * @throws SQLiteException, or an exception derived therefrom, if there
	 * is some other problem in preparing the statement, which results in a
	 * SQLite error code (that is not SQLITE_OK) being returned.
	 *
	 * @throws TooManyStatements if the first purported SQL statement
	 * in str is syntactically acceptable to SQLite, <em>but</em> there
	 * are characters in str after this statement, other than ';' and ' '.
	 * This includes the case where there are further syntactically
	 * acceptable SQL statements after the first one - as each SQLStatement
	 * can encapsulate only one statement.
	 *
	 * @throws std::bad_alloc in the extremely unlikely event of memory
	 * allocation failure in execution.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	boost::shared_ptr<detail::SQLStatement> provide_sql_statement
	(	std::string const& statement_text
	);

	/**
	 * Begins a SQL transaction. Transactions may be nested. Only the
	 * outermost call to begin_transaction causes the "begin transaction"
	 * SQL command to be executed. Inner calls instead cause a
	 * transaction savepoint to be set (see SQLite documentation
	 * re. savepoints).
	 *
	 * SQL transactions should be controlled either solely through the
	 * methods begin_transaction and end_transaction, \e or solely through
	 * the direct execution of SQL statement strings "begin transaction" and
	 * "end transaction". Mixing the two will result in undefined behaviour.
	 *
	 * @throws TransactionNestingException in the event that the maximum
	 * level of nesting has been reached. The maximum level of nesting is
	 * equal to the value returned by max_nesting();
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws std::bad_alloc in the extremely unlikely event of a memory
	 * allocation error in execution.
	 *
	 * Exception safety: the <em>strong guarantee</em> is provided, on the
	 * condition that the control of SQL transactions is managed
	 * entirely by calls to begin_transaction(), end_transaction() and
	 * canced_transaction(), rather than by executing the corresponding
	 * SQL commands directly.
	 *
	 * @todo Hide begin_transaction(), end_transaction() and
	 * cancel_transaction() from everyone except DatabaseTransaction.
	 */
	void begin_transaction();

	/**
	 * Ends a SQL transaction. Transactions may be nested. Only the outermost
	 * call to end_transaction causes the "end transaction" SQL command
	 * to be executed. Inner calls cause the previous savepoint to be
	 * released (see SQLite documentation re. savepoints).
	 *
	 * @throws TransactionNestingException in the event that there are
	 * more calls to end_transaction than there have been to
	 * begin_transaction - in other words, there are no active
	 * transactions.
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws std::bad_alloc in the extremely unlikely event of a memory
	 * allocation error in execution.
	 *
	 * Exception safety: as per begin_transaction().
	 */
	void end_transaction();

	/**
	 * Cancels a SQL transaction. If the active transaction is an
	 * outermost transaction, i.e. there is no nesting in the current
	 * transaction, then this causes the entire transaction to be rolled
	 * back. Otherwise, it causes a rollback to the last savepoint, AND
	 * the release of that savepoint. (See SQLite documentation for
	 * explanation of rollbacks, savepoints and releases of savepoints.)
	 *
	 * @throws TransactionNestingException if there is no open active
	 * transaction.
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws std::bad_alloc in the very unlikely event of a memory
	 * allocation failure in execution.
	 *
	 * Exception safety: as per begin_transaction().
	 */
	void cancel_transaction();

	void unchecked_begin_transaction();
	void unchecked_end_transaction();
	void unchecked_set_savepoint();
	void unchecked_release_savepoint();
	void unchecked_rollback_transaction();
	void unchecked_rollback_to_savepoint();

	boost::scoped_ptr<detail::SQLiteDBConn> m_sqlite_dbconn;

	// s_max_nesting relies on m_transaction_nesting_level being an int
	int m_transaction_nesting_level;
	static int const s_max_nesting;

	StatementCache m_statement_cache;
	StatementCache::size_type m_cache_capacity;
};




template <typename T, typename Connection>
inline
Handle<T>
get_handle(Connection& dbc, typename T::Id id)
{
	return dbc.template
		identity_map<T>().template
		provide_object(id);
}

template <typename T, typename Connection>
inline
Handle<T>
get_handle(Connection& dbc)
{
	return dbc.template
		identity_map<T>().template
		provide_object();
}



inline
boost::shared_ptr<detail::SQLStatement>
DatabaseConnection::StatementAttorney::provide_sql_statement
(	DatabaseConnection& p_database_connection,
	std::string const& p_statement_text
)
{
	return p_database_connection.provide_sql_statement
	(	p_statement_text
	);
}


inline
void
DatabaseConnection::TransactionAttorney::begin_transaction
(	DatabaseConnection& p_database_connection
)
{
	p_database_connection.begin_transaction();
	return;
}

inline
void
DatabaseConnection::TransactionAttorney::end_transaction
(	DatabaseConnection& p_database_connection
)
{
	p_database_connection.end_transaction();
	return;
}

inline
void
DatabaseConnection::TransactionAttorney::cancel_transaction
(	DatabaseConnection& p_database_connection
)
{
	p_database_connection.cancel_transaction();
	return;
}



}  // namespace sqloxx

#endif  // database_connection_hpp

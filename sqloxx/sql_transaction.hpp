#ifndef GUARD_sql_transaction_hpp
#define GUARD_sql_transaction_hpp

#include <boost/noncopyable.hpp>
#include "database_connection.hpp"

namespace sqloxx
{


/**
 * Sentry class to manage SQL transaction. Provides a RAII
 * wrapper around calls to DatabaseConnection::begin_transaction()
 * (called automatically in the constructor),
 * and DatabaseConnection::end_transaction() (called by calling
 * finish(), or otherwise called by the destructor when the
 * instance of SQLTransaction goes out of scope). Each SQLTransaction
 * instance is guaranteed to call end_transaction() only once,
 * regardless of the number of times (0 or more) finish() is
 * called on the SQLTransaction instance.
 *
 * The class is merely a "sentry" class for managing calls to
 * begin_transaction() and end_transaction(). The
 * SQL statements comprising the transaction should themselves still be
 * executed as normal via \e SharedSQLStatement functions (recommended), or
 * via \e DatabaseConnection::execute_sql. 
 *
 * @todo Unit testing.
 */
class SQLTransaction: public boost::noncopyable
{
public:

	/**
	 * @param dbc DatabaseConnection on which the transaction
	 * is to be executed.
	 *
	 * @throws TransactionNestingException in the event that the
	 * maximum level of nesting for \e dbc has been reached.
	 *
	 * @throws InvalidConnection if \e dbc is an invalid
	 * DatabaseConnection.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	SQLTransaction(DatabaseConnection& dbc);

	/**
	 * On destruction, end_transaction is called automatically
	 * on \e dbc, where \e dbc is the DatabaseConnection
	 * that was passed to the constructor. It is possible
	 * that the underlying call to e end_transaction() will fail,
	 * in which case this destructor will not throw, but will instead simply
	 * print the exception's error message to \e std::cerr, as it proceeds
	 * with destruction. In the unlikey event of a memory allocation
	 * failure while attempting to print the error message, no exception
	 * will be thrown, and no error message written.
	 *
	 * Note that if \e finish() has already successfully been called on this
	 * SQLTransaction instance, then the destructor will refrain from calling
	 * \e end_transaction() a second time.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	~SQLTransaction();

	/**
	 * Wraps a call to \e dbc.end_transaction(), where \e dbc is the
	 * DatabaseConnection that was passed to the constructor. If \e finish()
	 * is called multiple times on the same SQLTransaction instance, only the
	 * first call will result in \e end_transaction() being called; the others
	 * will have no effect.
	 *
	 * @throws TransactionNestingException in hte event that there are more
	 * calls to end_transaction on \e dbc than there have been to
	 * begin_transaction(). (Note that if SQL transactions are always managed
	 * via SQLTransaction objects, this can never occur. It would only occur
	 * via direct calls to \e DatabaseConnection::begin_transaction() and \e
	 * DatabaseConnnection::end_transaction().)
	 *
	 * @throws InvalidConnection if the database connection that was passed to
	 * the constructor is now invalid.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	void finish();

private:
	bool m_is_active;
	DatabaseConnection& m_database_connection;
};


}  // namespace sqloxx


#endif  // GUARD_sql_transaction_hpp

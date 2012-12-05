#ifndef GUARD_database_transaction_hpp
#define GUARD_database_transaction_hpp

#include "database_connection.hpp"
#include <boost/noncopyable.hpp>

namespace sqloxx
{

/**
 * Class for managing database transactions using RAII.
 * An instance of DatabaseTransaction, when created, causes
 * an SQL transaction to be commenced on the DatabaseConnection with
 * which it is initialized - or, if nested within another transaction, a
 * savepoint will be set.
 * The SQL transaction can then
 * be finalized in one of three ways:\n
 * (a) We call the DatabaseTransaction's commit() method, causing
 * the transaction to be committed (or, if nested, the previous
 * savepoint to be released);\n
 * (b) We call the DatabaseTransaction's cancel() method, causing
 * the transaction to be cancelled i.e. rolled back (or, if nested,
 * the transaction is rolled back to the last savepoint and that
 * savepoint released);\n
 * (c) The DatabaseTransaction is destruced without commit() or
 * cancel() having been called - in which case the SQL transaction
 * is cancelled i.e. rolled back (or, if nested, the transaction is
 * rolled back to the last savepoint and that savepoint is released).
 *
 * (c) is intended more as a safeguard against the programmer forgetting to
 * call cancel() manually, than as
 * than a way to avoid having calling cancel() manually. This is because
 * it is conceivable (though extremely unlikely), that the destructor
 * might fail to cancel the transaction - in which case std::terminate() is
 * called rather than throw an exception from the destructor.
 *
 * Preconditions:\n
 * The management of database transactions must be managed entirely using
 * instances of the DatabaseTransaction class, rather than by executing
 * SQL commands "begin transaction", "savepoint" etc. directly.
 * The behaviour documented
 * in the member function documentation for this class depends on this
 * being the case.
 *
 * @todo Documentation and testing.
 */
class DatabaseTransaction:
	public boost::noncopyable
{
public:

	/**
	 * Preconditions: see documentation for class.
	 *
	 * Creates an object serving as a sentry for a database transaction.
	 * The constructor causes a transaction to be commenced - or, if there
	 * is already an active transaction, a savepoint to be set. (See SQLite
	 * documentation regarding the effect of setting a savepoint.)
	 *
	 * @throws TransactionNestingException in the extremely unlikely
	 * event that the maximum
	 * level of nesting has been reached. The maximum level of nesting is
	 * equal to the value returned by the max_nesting() method of
	 * the database connection.
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws std::bad_alloc in the extremely unlikely event of a memory
	 * allocation error in execution.
	 *
	 * Exception safety: the <em>strong guarantee</em> is provided, on the
	 * condition that the control of database transactions is managed
	 * entirely via the DatabaseTransaction class, rather than by
	 * executing the SQL commands ("begin transaction", "end transaction"
	 * etc.) directly.
	 * 
	 * @todo Testing.
	 */
	explicit DatabaseTransaction(DatabaseConnection& p_database_connection);

	/**
	 * Preconditions: see documentation for class.
	 *
	 * Under normal circumstances, the destructor will cause the database
	 * transaction to be cancelled, if it is still active, via a
	 * call to cancel(). (See separate documentation to cancel() for more
	 * detail.) If it has already
	 * been cancelled or committed, the destructor will simply destroy
	 * the DatabaseTransaction object with no other effects.
	 *
	 * Note however, that the destructor, unlike cancel(), will never throw.
	 * If the internal call to cancel() throws, then the destructor will
	 * print an error message, and call std::terminate. For this reason,
	 * client code should not rely on the destructor, but should call
	 * cancel() explicitly. The destructor is rather a balwark against
	 * the programmer forgetting to call cancel() manually.
	 *
	 * Exception safety: <em>nothrow guarantee, but might call
	 * std::terminate()</em>.
	 */
	~DatabaseTransaction();

	/**
	 * Preconditions: see documentation for class. In addition, if
	 * UnresolvedTransactionException is thrown, the application must
	 * exit without further commands being executed on the database
	 * connection.
	 *
	 * Causes the <em>innermost currently active</em>
	 * DatabaseTransaction's transaction to be committed to the
	 * database.
	 * <em>In theory, this may not be the same DatabaseTransaction as
	 * the instance on which we are calling commit().</em> In practice,
	 * we would have to write some pretty perverse code in order for it not
	 * to be the same DatabaseTransaction.
	 *
	 * If the innermost currently active DatabaseTransaction is an outer
	 * transaction, i.e. is not nested within any other DatabaseTransaction,
	 * then the call commit() causes the entire underlying database
	 * transaction to be committed to the database.
	 *
	 * If the innermost currently active DatabaseTransacton is nested
	 * within one or more other DatabaseTransactions, then the call to
	 * commit() causes the most recent savepoint to be released.
	 * (See SQLite documentation in relation
	 * to the effect of releasing a savepoint.)
	 *
	 * @throws TransactionNestingException if called on an inactive
	 * DatabaseTransaction (i.e. one on which cancel() or commit() has
	 * already been called).
	 *
	 * @throws UnresolvedTransactionException if the DatabaseConnection
	 * (on which this DatabaseTransaction was initialized) is invalid, or
	 * if there has been a memory allocation failure in attempting to wind
	 * up the transaction. If this happens, then the transaction will not
	 * be committed to the database; rollback is assured, on condition that
	 * the program exits without further SQL being executed on the
	 * database connection.
	 *
	 * Exception safety: <em>strong guarantee</em>, provided the preconditions
	 * are met.
	 *
	 * @todo Testing.
	 */
	void commit();

	/**
	 * Preconditions: see documentation for class. In addition, if
	 * UnresolvedTransactionException is thrown, the application must
	 * exit without further commands being executed on the database
	 * connection.
	 *
	 * Causes the <em>innermost currently active</em>
	 * DatabaseTransaction's transaction to be cancelled. 
	 * <em>In theory, this may not be the same DatabaseTransaction as
	 * the instance on which we are calling commit().</em> In practice,
	 * we would have to write some pretty perverse code in order for it not
	 * to be the same DatabaseTransaction.
	 *
	 * If the innermost currently active DatabaseTransaction is an outer
	 * transaction, i.e. is not nested within any other DatabaseTransaction,
	 * then the call to cancel() causes the entire underlying database
	 * transaction to be cancelled.
	 *
	 * If the innermost currently active DatabaseTransacton is nested
	 * within one or more other DatabaseTransactions, then the call to
	 * cancel() causes a rollback to the most recent savepoint, followed
	 * by the release of that savepoint.
	 * (See SQLite documentation in relation
	 * to the effect of releasing a savepoint.)
	 *
	 * @throws TransactionNestingException if called on an inactive
	 * DatabaseTransaction (i.e. one on which cancel() or commit() has
	 * already been called).
	 *
	 * @throws UnresolvedTransactionException if the DatabaseConnection
	 * (on which this DatabaseTransaction was initialized) is invalid, or
	 * if there has been a memory allocation failure in attempting to wind
	 * up the transaction. If this happens, then the transaction will not
	 * be committed to the database; rollback is assured, on condition that
	 * the program exits without further SQL being executed on the
	 * database connection.
	 *
	 * Exception safety: <em>strong guarantee</em>, provided the preconditions
	 * are met.
	 *
	 * @todo Testing.
	 */
	void cancel();

private:
	bool m_is_active;
	DatabaseConnection& m_database_connection;

};  // DatabaseTransaction


}  // sqloxx

#endif  // GUARD_database_transaction_hpp

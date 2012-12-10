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
 * Note the facilities provided by DatabaseTransaction are importantly
 * limited. They provide transactionality/atomicity in respect of interactions
 * with a database. However, they do not magically provide such atomicity
 * in respect of client operations on in-memory objects - even if these
 * in-memory
 * objects are objects that \e represent records in a database.
 * So, suppose some client code commences a DatabaseTransaction; then saves
 * five new PersistentObject instances (via their handles)
 * to the database; and then calls
 * cancel() on the DatabaseTransaction. The database state will indeed be
 * rolled back. However the five in-memory objects and their handles
 * will continue having
 * the same state as if they had been successfully saved to the database
 * (including
 * having an id retrievable by calling their id() method). In particular,
 * a call to unchecked_provide_handle(Id) or unchecked_get_handle(dbc, Id)
 * passing one of these "lingering" ids to the Id parameter,
 * may even silently return a handle
 * to one of these objects if it is still "lingering" in the IdentityMap
 * (cache).
 * To erase all trace of the lingering objects from the cache,
 * the client can call remove() on the lingering objects, but this can only
 * be done manually - the DatabaseTransaction cancellation does not
 * achieve this automatically.
 * Having said this, even if the client leaves the objects lingering
 * in the cache, this is \e not a problem for the integrity of the
 * cache. It's only a problem if the lingering objects are referred to
 * or otherwise used after their saving has been cancelled. The client
 * should in any case not call unchecked_provide_handle(...) or
 * unchecked_get_handle(...) unless they know they have a valid id.
 *
 * Despite the above warning, note that many functions in Sqloxx \e do
 * offer the
 * strong guarantee - i.e. atomicity - in respect of both in-memory objects
 * and the physical database; and many of these functions happen to use
 * DatabaseTransaction internally to help achieve this.
 * The point is not
 * that Sqloxx operations are never atomic in regards to in-memory
 * objects; rather, the point is that wrapping arbitrary code in a
 * DatabaseTransaction will not make it atomic in and of itself - even
 * if that code consists solely of a series of Sqloxx API calls such
 * as save(), remove() etc..
 *
 * Preconditions:\n
 * The management of database transactions must be managed entirely using
 * instances of the DatabaseTransaction class, rather than by executing
 * SQL commands "begin transaction", "savepoint" etc. directly.
 * The behaviour documented
 * in the member function documentation for this class depends on this
 * being the case.
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
	 */
	void cancel();

private:
	bool m_is_active;
	DatabaseConnection& m_database_connection;

};  // DatabaseTransaction


}  // sqloxx

#endif  // GUARD_database_transaction_hpp

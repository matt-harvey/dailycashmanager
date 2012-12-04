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
 * @todo Documentation and testing.
 */
class DatabaseTransaction:
	public boost::noncopyable
{
public:

	/**
	 * Creates an object serving as a sentry for a database transaction.
	 * The constructor causes a transaction to be commenced - or, if there
	 * is already an active transaction, a savepoint to be set.
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

	~DatabaseTransaction();
	void commit();
	void cancel();
private:
	bool m_is_active;
	DatabaseConnection& m_database_connection;

};  // DatabaseTransaction


}  // sqloxx

#endif  // GUARD_database_transaction_hpp

#ifndef GUARD_database_transaction_hpp
#define GUARD_database_transaction_hpp

#include "database_connection.hpp"
#include <boost/noncopyable.hpp>

namespace sqloxx
{

/**
 * Class for managing database transactions using RAII.
 * An instance of DatabaseTransaction, when created, causes
 * an SQL transaction to commenced on the DatabaseConnection with
 * which it is initialized. The SQL transaction can then
 * be finalized in one of three ways:\n
 * (a) We call the DatabaseTransaction's commit() method, causing
 * the transaction to be committed;
 * (b) We call the DatabaseTransaction's cancel() method, causing
 * the tranaction to be cancelled i.e. rolled back, or
 * (c) The DatabaseTransaction is destruced without commit() or
 * cancel() having been called - in which case the SQL transaction
 * is cancelled i.e. rolled back.
 *
 * (c) is intended as a balwark against programmer error, rather
 * than a way to avoid calling cancel() manually. See further
 * documentation of each for why.
 *
 * @todo Documentation and testing.
 */
class DatabaseTransaction:
	public boost::noncopyable
{
public:
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

#ifndef sql_statement_manager_hpp
#define sql_statement_manager_hpp

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>


namespace sqloxx
{

class DatabaseConnection;
class SQLStatement;

/**
 * @todo Document, test, etc...
 *
 * @todo Make an instance of SQLStatementManager
 * be a member of DatabaseConnection - or possibly
 * of a wrapper around DatabaseConnection. We have
 * to watch out for circular references here. Then
 * the clients will only have to manage a single
 * DatabaseConnection class, rather than having to
 * separately manage the SQLStatementManager as well.
 * Then SharedSQLStatement, just like SQLStatement,
 * can have a constructor that accepts a reference
 * to the DatabaseConnection class.
 */
class SQLStatementManager
{

public:

	typedef
		std::map< std::string, boost::shared_ptr<SQLStatement> >
		StatementCache;

	explicit
	SQLStatementManager
	(	boost::shared_ptr<DatabaseConnection> p_database_connection,
		StatementCache::size_type p_capacity
	);
	
	/**
	 * @returns a smart pointer to a SQLStatement allocated on the free store.
	 *
	 * @todo This should return some kind of handler than manages the
	 * resetting of the statement. This avoids the current situation, in which
	 * the client must remember to reset the statement when they are done with
	 * it, or else the statement hangs around in a non-reset state,
	 * potentially hogging resources (which probably doesn't matter, but
	 * is unnecessary and avoidable, and recommended against by Kreibich
	 * in "Using SQLite".
	 */
	boost::shared_ptr<SQLStatement> provide_sql_statement
	(	std::string const& statement_text
	);
	

private:
	
	boost::shared_ptr<DatabaseConnection> m_database_connection;
	StatementCache m_statement_cache;
	StatementCache::size_type m_capacity;
	
};



}  // namespace sqloxx

#endif  // sql_statement_manager_hpp

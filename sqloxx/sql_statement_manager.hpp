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
 * @todo HIGH PRIORITY Change the SQLStatement class to actually
 * make use of this by enabling resets etc.. Change client code
 * in Phatbooks if required.
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

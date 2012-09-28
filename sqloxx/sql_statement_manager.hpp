#ifndef sql_statement_manager_hpp
#define sql_statement_manager_hpp

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <string>


namespace sqloxx
{

class SQLiteDBConn;
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
class SQLStatementManager:
	private boost::noncopyable
{
public:

	typedef
		boost::unordered_map< std::string, boost::shared_ptr<SQLStatement> >
		StatementCache;

	explicit
	SQLStatementManager
	(	boost::shared_ptr<SQLiteDBConn> p_sqlite_dbconn,
		StatementCache::size_type p_capacity
	);
	
	/**
	 * @returns a smart pointer to a SQLStatement allocated on the free store.
	 */
	boost::shared_ptr<SQLStatement> provide_sql_statement
	(	std::string const& statement_text
	);
	

private:
	
	boost::shared_ptr<SQLiteDBConn> m_sqlite_dbconn;
	StatementCache m_statement_cache;
	StatementCache::size_type m_capacity;
	
};



}  // namespace sqloxx

#endif  // sql_statement_manager_hpp

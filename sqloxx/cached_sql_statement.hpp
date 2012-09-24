#ifndef cached_sql_statement_hpp
#define cached_sql_statement_hpp

#include <boost/shared_ptr.hpp>
#include <string>


namespace sqloxx
{

class SQLStatement;
class SQLStatementManager;


/**
 * Class to handle SQLStatement instances via a cache. The
 * caching is done by SQLStatementManager. When a
 * CachedSQLStatement is destructed, it is automatically
 * reset. However the underlying SQLStatement remains in
 * the cache ready for deployment (unless the cache is full).
 * The details of caching are managed by the SQLStatementManager.
 * The client of CachedSQLStatement just calls the constructor
 * and uses the statement.
 *
 * @todo HIGH PRIORITY Provide the means for calling the usual
 * methods in SQLStatement, i.e. bind, step etc.. These should
 * simply be delegated to the underlying SQLStatement. The
 * CachedSQLStatement is simply a handler.
 *
 * @todo HIGH PRIORITY Within the Phatbooks client code, we
 * need to incorporate a SQLStatementManager. This should
 * probably be a constituent of a "session" object. Each
 * "session" can have a DatabaseConnection and a
 * SQLStatementManager. These should be members of a
 * PhatbooksSession base class. PhatbooksTextSession and
 * PhatbooksGUISession (?) can then be derived from this
 * base class.
 */
class CachedSQLStatement
{
public:
	
	CachedSQLStatement
	(	SQLStatementManager& p_sql_statement_manager,
		std::string const& p_statement_text
	);

	~CachedSQLStatement();

private:

	boost::shared_ptr<SQLStatement> m_sql_statement;
};


}  // namespace sqloxx

#endif  // cached_sql_statement_hpp

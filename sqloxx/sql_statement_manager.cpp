#include "sql_statement_manager.hpp"
#include "sql_statement.hpp"
#include <boost/circular_buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

using boost::shared_ptr;

namespace sqloxx
{


SQLStatementManager::SQLStatementManager
(	boost::shared_ptr<DatabaseConnection> p_database_connection,
	History::size_type p_cache_capacity
):
	m_database_connection(p_database_connection),
	m_statement_history(p_cache_capacity)
{
}


void
SQLStatementManager::add_statement(shared_ptr<SQLStatement> p_statement)
{
	// WARNING Incomplete
	
	// Add p_statement to m_statement_history
	// If the m_statement_history overflowed, then we must remove
	// the jettisoned statement from m_statement_cache also.
	// Add p_statement to m_statement_cache, indexed by the text of the
	// statement. (How do we know what that is?)

}


	










}  // namespace sqloxx

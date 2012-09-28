#include "sql_statement_manager.hpp"
#include "sql_statement.hpp"
#include <boost/circular_buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/debug_log.hpp>
#include <cassert>
#include <iostream>  // For debug logging
#include <string>

using boost::shared_ptr;
using boost::unordered_map;
using std::endl;  // For debug logging
using std::string;

namespace sqloxx
{


SQLStatementManager::SQLStatementManager
(	shared_ptr<SQLiteDBConn> p_sqlite_dbconn,
	StatementCache::size_type p_capacity
):
	m_sqlite_dbconn(p_sqlite_dbconn),
	m_capacity(p_capacity)
{
}


shared_ptr<SQLStatement>
SQLStatementManager::provide_sql_statement(string const& statement_text)
{
	StatementCache::const_iterator const it =
		m_statement_cache.find(statement_text);
	if (it != m_statement_cache.end())
	{
		JEWEL_DEBUG_LOG << "Returning shared SQLStatement." << endl;
		return it->second;
	}
	assert (it == m_statement_cache.end());
	shared_ptr<SQLStatement> statement
	(	new SQLStatement(*m_sqlite_dbconn, statement_text)
	);
	if (m_statement_cache.size() != m_capacity)
	{
		assert (m_statement_cache.size() < m_capacity);
		JEWEL_DEBUG_LOG << "Storing new SQLStatement in cache." << endl;
		m_statement_cache[statement_text] = statement;
	}
	else
	{
		JEWEL_DEBUG_LOG << "SQLStatement cache has reached capacity, "
		                << "and caching has been discontinued."
						<< endl;
	}
	return statement;
}




}  // namespace sqloxx

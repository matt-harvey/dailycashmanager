#include "cached_sql_statement.hpp"
#include "sql_statement.hpp"
#include "sql_statement_manager.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::string;


namespace sqloxx
{


CachedSQLStatement::CachedSQLStatement
(	SQLStatementManager& p_sql_statement_manager,
	string const& p_statement_text
):
	m_sql_statement
	(	p_sql_statement_manager.provide_sql_statement(p_statement_text)
	)
{
}


CachedSQLStatement::~CachedSQLStatement()
{
	m_sql_statement->reset();
}



}  // namespace sqloxx

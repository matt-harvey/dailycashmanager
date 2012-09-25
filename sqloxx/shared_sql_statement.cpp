#include "shared_sql_statement.hpp"
#include "sql_statement.hpp"
#include "sql_statement_manager.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::string;


namespace sqloxx
{


SharedSQLStatement::SharedSQLStatement
(	SQLStatementManager& p_sql_statement_manager,
	string const& p_statement_text
):
	m_sql_statement
	(	p_sql_statement_manager.provide_sql_statement(p_statement_text)
	)
{
}



}  // namespace sqloxx

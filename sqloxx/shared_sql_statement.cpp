#include "shared_sql_statement.hpp"
#include "database_connection.hpp"
#include "detail/sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::string;


namespace sqloxx
{


SharedSQLStatement::SharedSQLStatement
(	DatabaseConnection& p_database_connection,
	string const& p_statement_text
):
	m_sql_statement
	(	p_database_connection.provide_sql_statement(p_statement_text)
	)
{
}

SharedSQLStatement::~SharedSQLStatement()
{
	m_sql_statement->reset();
	m_sql_statement->clear_bindings();
	m_sql_statement->unlock();
}



}  // namespace sqloxx

#include "sql_statement.hpp"
#include <string>

using std::string;


namespace sqloxx
{

SQLStatement::SQLStatement():
	m_statement(0)
{
}

SQLStatement::~SQLStatement()
{
	if (m_statement != 0)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
	}
}

void
SQLStatement::prepare(DatabaseConnection& dbconn, string const& str)
{
	int return_code = sqlite3_prepare_v2
	(	dbconn.m_connection,
		str.c_str(),
		-1,
		&m_statement,
		0
	);
	if (return_code != SQLITE_OK)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
		dbconn.throw_sqlite_exception();
	}
	return;
}


}  // namespace sqloxx

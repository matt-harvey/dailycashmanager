#include "sql_statement.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <string>

using std::string;


namespace sqloxx
{

SQLStatement::SQLStatement(DatabaseConnection& dbconn, string const& str):
	m_statement(0)
{
	if (!dbconn.is_valid())
	{
		throw SQLiteException
		(	"Attempt to initialize SQLStatement with invalid "
			"DatabaseConnection."
		);
	}
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





SQLStatement::~SQLStatement()
{
	if (m_statement != 0)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
	}
}


}  // namespace sqloxx

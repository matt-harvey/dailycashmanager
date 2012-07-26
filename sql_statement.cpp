#include "sql_statement.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <sqlite3.h>
#include <cassert>
#include <string>

using std::string;


namespace sqloxx
{

SQLStatement::SQLStatement(DatabaseConnection& dbconn, string const& str):
	m_statement(0),
	m_database_connection(dbconn)
{
	if (!dbconn.is_valid())
	{
		throw SQLiteException
		(	"Attempt to initialize SQLStatement with invalid "
			"DatabaseConnection."
		);
	}
	int const return_code = sqlite3_prepare_v2
	(	m_database_connection.m_connection,
		str.c_str(),
		-1,
		&m_statement,
		0
	);
	check_ok(return_code);
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

void
SQLStatement::check_ok(int err_code)
{
	if (err_code != SQLITE_OK)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
		m_database_connection.throw_sqlite_exception();
	}
	return;
}

void
SQLStatement::bind(std::string const& parameter_name, int value)
{
	int const index = parameter_index(parameter_name);
	int const return_code = sqlite3_bind_int(m_statement, index, value);
	check_ok(return_code);
	return;
}


void
SQLStatement::bind(std::string const& parameter_name, std::string const& str)
{
	int const index = parameter_index(parameter_name);
	int const return_code = sqlite3_bind_text
	(	m_statement,
		index,
		str.c_str(),
		-1,
		0
	);
	check_ok(return_code);
	return;
}
		

bool
SQLStatement::step()
{
	int const return_code = sqlite3_step(m_statement);
	if (return_code == SQLITE_DONE)
	{
		return false;
	}
	if (return_code == SQLITE_ROW)
	{
		return true;
	}
	assert ( (return_code != SQLITE_DONE) && (return_code != SQLITE_ROW) );
	m_database_connection.throw_sqlite_exception();
	assert (false);  // Execution should never reach here.
	return false;  // Silence compiler re. return from non-void function. 
}

int
SQLStatement::parameter_index(std::string const& parameter_name) const
{
	int const ret = sqlite3_bind_parameter_index
	(	m_statement,
		parameter_name.c_str()
	);
	if (ret == 0) 
	{
		throw SQLiteException("Could not find parameter index.");
	}
	assert (ret > 0);
	return ret;
}



}  // namespace sqloxx

#include "sql_statement.hpp"
#include "sqlite_dbconn.hpp"

#include <sqlite3.h>
#include <string>

using std::string;


namespace sqloxx
{
namespace detail
{



SQLStatement::SQLStatement
(	SQLiteDBConn& p_sqlite_dbconn,
	string const& str
):
	m_statement(0),
	m_sqlite_dbconn(p_sqlite_dbconn)
{
	if (!p_sqlite_dbconn.is_valid())
	{
		throw InvalidConnection
		(	"Attempt to initialize SQLStatement with invalid "
			"DatabaseConnection."
		);
	}
	sqlite3_prepare_v2
	(	m_sqlite_dbconn.m_connection,
		str.c_str(),
		-1,
		&m_statement,
		0
	);
	check_ok();
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
SQLStatement::check_column(int index, int value_type)
{
	int const num_columns = sqlite3_column_count(m_statement);
	if (num_columns == 0)
	{
		throw NoResultRowException("Result row not available.");
	}
	if (index > num_columns)
	{
		throw ResultIndexOutOfRange("Index is out of range.");
	}
	if (index < 0)
	{
		throw ResultIndexOutOfRange("Index is negative.");
	}
	if (value_type != sqlite3_column_type(m_statement, index))
	{
		throw ValueTypeException
		(	"Value type at index does not match specified value type."
		);
	}
	return;
}


void
SQLStatement::check_ok()
{
	try
	{
		m_sqlite_dbconn.check_ok();
	}
	catch (SQLiteException&)
	{
		sqlite3_finalize(m_statement);
		m_statement = 0;
		throw;
	}
	return;
}

void
SQLStatement::bind
(	std::string const& parameter_name,
	int value
)
{
	sqlite3_bind_int(m_statement, parameter_index(parameter_name), value);
	check_ok();
	return;
}


void
SQLStatement::bind
(	string const& parameter_name,
	int64_t value
)
{
	sqlite3_bind_int64(m_statement, parameter_index(parameter_name), value);
	check_ok();
	return;
}


void
SQLStatement::bind
(	string const& parameter_name,
	string const& str
)
{
	sqlite3_bind_text
	(	m_statement,
		parameter_index(parameter_name),
		str.c_str(),
		-1,
		0
	);
	check_ok();
	return;
}
		

bool
SQLStatement::step()
{
	switch (sqlite3_step(m_statement))
	{
	case SQLITE_DONE:
		return false;
		assert (false);  // Execution never reaches here
	case SQLITE_ROW:
		return true;
		assert (false);  // Execution never reaches here
	default:
		;
		// Do nothing
	}
	check_ok();
	assert (false);  // Execution should never reach here.
	return false;  // Silence compiler re. return from non-void function. 
}


void
SQLStatement::step_last()
{
	if (step())
	{
		sqlite3_finalize(m_statement);
		throw UnexpectedResultRow
		(	"Statement yielded a result set when none was expected."
		);
	}
	return;
}


void
SQLStatement::reset()
{
	sqlite3_reset(m_statement);
	return;
}


int
SQLStatement::parameter_index
(	string const& parameter_name
)
const
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



}  // namespace detail
}  // namespace sqloxx

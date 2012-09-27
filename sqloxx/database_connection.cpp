#include "database_connection.hpp"
#include "sqlite_dbconn.hpp"
#include "sqloxx_exceptions.hpp"
#include "sql_statement.hpp"
#include "sql_statement_manager.hpp"
#include <iostream>
#include <string>
#include <vector>

using std::clog;
using std::endl;
using std::string;
using std::vector;

namespace sqloxx
{


namespace
{
	int const cache_capacity = 200;
}


DatabaseConnection::DatabaseConnection():
	m_sqlite_dbconn(new SQLiteDBConn),
	m_sql_statement_manager
	(	new SQLStatementManager(m_sqlite_dbconn, cache_capacity)
	),
	m_transaction_nesting_level(0)
{
}


DatabaseConnection::~DatabaseConnection()
{
	if (m_transaction_nesting_level > 0)
	{
		clog << "Transaction(s) remained incomplete on closure of "
		     << "DatabaseConnection."
			 << endl;
	}
}


bool
DatabaseConnection::is_valid() const
{
	return m_sqlite_dbconn->is_valid();
}


void
DatabaseConnection::open(boost::filesystem::path const& filepath)
{
	m_sqlite_dbconn->open(filepath);
	return;
}


void
DatabaseConnection::execute_sql(string const& str)
{
	m_sqlite_dbconn->execute_sql(str);
	return;
}


void
DatabaseConnection::check_ok()
{
	m_sqlite_dbconn->check_ok();
	return;
}


void
DatabaseConnection::setup_boolean_table()
{
	m_sqlite_dbconn->setup_boolean_table();
	return;
}


void
DatabaseConnection::begin_transaction()
{
	if (!m_transaction_nesting_level)
	{
		assert (m_transaction_nesting_level == 0);
		SharedSQLStatement statement
		(	*this,
			"begin"
		);
		statement.step();
	}
	++m_transaction_nesting_level;
	return;
}


void
DatabaseConnection::end_transaction()
{
	switch (m_transaction_nesting_level)
	{
	case 1:
		unchecked_end_transaction();
		break;
	case 0:
		throw TransactionNestingException
		(	"Number of transactions ended on this database connection "
			"exceeds the number of transactions begun."
		);
		assert (false);  // Execution never reaches here
	default:
		;
		// Do nothing
	}
	assert (m_transaction_nesting_level > 0);
	--m_transaction_nesting_level;
	return;
}


boost::shared_ptr<SQLStatement>
DatabaseConnection::provide_sql_statement(string const& statement_text)
{
	return m_sql_statement_manager->provide_sql_statement
	(	statement_text
	);
}


vector<string>
DatabaseConnection::primary_key(string const& table_name)
{
	static int const pk_info_field = 5;
	static int const column_name_field = 1;
	vector<string> ret;
	SharedSQLStatement statement
	(	*this,
		"pragma table_info(" + table_name + ")"
	);
	bool steps_remain = true;
	// Assignment operator is deliberate here
	while ((steps_remain = statement.step()))
	{
		if (statement.extract<int>(pk_info_field) == 1)
		{
			ret.push_back(statement.extract<string>(column_name_field));
		}
	}
	assert (!steps_remain);
	return ret;
}


}  // namespace sqloxx

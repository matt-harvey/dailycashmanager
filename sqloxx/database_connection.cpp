#include "database_connection.hpp"
#include "sqlite_dbconn.hpp"
#include "sql_statement_manager.hpp"
#include <string>
#include <vector>

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
	)
{
}


DatabaseConnection::~DatabaseConnection()
{
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
	m_sqlite_dbconn->begin_transaction();
	return;
}


void
DatabaseConnection::end_transaction()
{
	m_sqlite_dbconn->end_transaction();
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

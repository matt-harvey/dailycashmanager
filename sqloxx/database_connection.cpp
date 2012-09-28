#include "database_connection.hpp"
#include "detail/sqlite_dbconn.hpp"
#include "sqloxx_exceptions.hpp"
#include "detail/sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <string>
#include <vector>


using boost::shared_ptr;
using boost::unordered_map;
using std::clog;
using std::endl;
using std::string;
using std::vector;

namespace sqloxx
{

DatabaseConnection::DatabaseConnection
(	StatementCache::size_type p_cache_capacity
):
	m_sqlite_dbconn(new detail::SQLiteDBConn),
	m_transaction_nesting_level(0),
	m_cache_capacity(p_cache_capacity)
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


void
DatabaseConnection::setup_boolean_table()
{
	execute_sql("create table booleans(representation integer primary key)");
	execute_sql("insert into booleans(representation) values(0)");
	execute_sql("insert into booleans(representation) values(1)");
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


shared_ptr<detail::SQLStatement>
DatabaseConnection::provide_sql_statement(string const& statement_text)
{
	StatementCache::const_iterator const it =
		m_statement_cache.find(statement_text);
	if (it != m_statement_cache.end())
	{
		return it->second;
	}
	assert (it == m_statement_cache.end());
	shared_ptr<detail::SQLStatement> statement
	(	new detail::SQLStatement(*m_sqlite_dbconn, statement_text)
	);
	if (m_statement_cache.size() != m_cache_capacity)
	{
		assert (m_statement_cache.size() < m_cache_capacity);
		m_statement_cache[statement_text] = statement;
	}
	else
	{
		// Cache has reached capacity and caching has been
		// discontinued.
	}
	return statement;
}

}  // namespace sqloxx

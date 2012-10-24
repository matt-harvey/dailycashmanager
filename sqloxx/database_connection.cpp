#include "database_connection.hpp"
#include "detail/sqlite_dbconn.hpp"
#include "sqloxx_exceptions.hpp"
#include "detail/sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <limits>
#include <set>
#include <string>


using boost::shared_ptr;
using boost::unordered_map;
using std::clog;
using std::endl;
using std::numeric_limits;
using std::set;
using std::string;

namespace sqloxx
{


int const
DatabaseConnection::s_max_nesting = numeric_limits<int>::max();


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
	m_statement_cache.clear();
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
	if (m_transaction_nesting_level == s_max_nesting)
	{
		throw TransactionNestingException("Maximum nesting level reached.");
	}
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


shared_ptr<detail::SQLStatement>
DatabaseConnection::provide_sql_statement(string const& statement_text)
{
	StatementCache::const_iterator const it
	(	m_statement_cache.find(statement_text)
	);
	if (it != m_statement_cache.end())
	{
		shared_ptr<detail::SQLStatement> existing_statement(it->second);
		if (!(existing_statement->is_locked()))
		{
			existing_statement->lock();
			return existing_statement;
		}
	}
	assert (it == m_statement_cache.end() || it->second->is_locked());
	shared_ptr<detail::SQLStatement> new_statement
	(	new detail::SQLStatement(*m_sqlite_dbconn, statement_text)
	);
	new_statement->lock();
	if (m_statement_cache.size() != m_cache_capacity)
	{
		assert (m_statement_cache.size() < m_cache_capacity);
		m_statement_cache[statement_text] = new_statement;
	}
	/*
	else
	{
		// Cache has reached capacity and caching has been
		// discontinued.
	}
	*/
	return new_statement;
}


}  // namespace sqloxx

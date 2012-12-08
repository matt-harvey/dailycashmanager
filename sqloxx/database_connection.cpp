#include "database_connection.hpp"
#include "database_transaction.hpp"
#include "detail/sqlite_dbconn.hpp"
#include "sql_statement.hpp"
#include "sqloxx_exceptions.hpp"
#include "detail/sql_statement_impl.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <climits>
#include <cstdio>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>

#include <jewel/debug_log.hpp>

using boost::shared_ptr;
using boost::unordered_map;
using std::bad_alloc;
using std::clog;
using std::endl;
using std::fprintf;
using std::numeric_limits;
using std::set;
using std::string;

namespace sqloxx
{


// Switch statement later relies on this being INT_MAX, and
// won't compile if it's changed to std::numeric_limits<int>::max().
int const
DatabaseConnection::s_max_nesting = INT_MAX;


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
		// We avoid streams here, because they might throw
		// (in theory, if exceptions have been enabled for the
		// stream).
		fprintf
		(	stderr,
			"Transaction(s) remained incomplete on closure of "
			"DatabaseConnection.\n"
		);
	}
	m_statement_cache.clear();
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
DatabaseConnection::setup_boolean_table()
{
	execute_sql("create table booleans(representation integer primary key)");
	execute_sql("insert into booleans(representation) values(0)");
	execute_sql("insert into booleans(representation) values(1)");
	return;
}


int
DatabaseConnection::max_nesting()
{
	return s_max_nesting;
}


void
DatabaseConnection::begin_transaction()
{
	switch (m_transaction_nesting_level)
	{
	case 0:
		unchecked_begin_transaction();
		break;
	case s_max_nesting:
		throw TransactionNestingException("Maximum nesting level reached.");
		assert (false);  // Execution never reaches here
	default:
		assert (m_transaction_nesting_level > 0);
		unchecked_set_savepoint();
		break;
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
		(	"Cannot end SQL transaction when there in none open."
		);
		assert (false);  // Execution never reaches here
	default:
		assert (m_transaction_nesting_level > 1);
		unchecked_release_savepoint();
		break;
	}
	assert (m_transaction_nesting_level > 0);
	--m_transaction_nesting_level;
	return;
}

void
DatabaseConnection::cancel_transaction()
{
	switch (m_transaction_nesting_level)
	{
	case 1:
		unchecked_rollback_transaction();
		break;
	case 0:
		throw TransactionNestingException
		(	"Cannot cancel SQL transaction when there is none open."
		);
		assert (false);  // Execution never reaches here
	default:
		assert (m_transaction_nesting_level > 1);
		unchecked_rollback_to_savepoint();
		unchecked_release_savepoint();
		break;
	}
	--m_transaction_nesting_level;
	return;
}

shared_ptr<detail::SQLStatementImpl>
DatabaseConnection::provide_sql_statement(string const& statement_text)
{
	if (!is_valid())
	{
		throw InvalidConnection("Invalid database connection.");
	}
	StatementCache::const_iterator const it
	(	m_statement_cache.find(statement_text)
	);
	if (it != m_statement_cache.end())
	{
		shared_ptr<detail::SQLStatementImpl> existing_statement(it->second);
		if (!(existing_statement->is_locked()))
		{
			existing_statement->lock();
			return existing_statement;
		}
	}
	assert (it == m_statement_cache.end() || it->second->is_locked());
	shared_ptr<detail::SQLStatementImpl> new_statement
	(	new detail::SQLStatementImpl(*m_sqlite_dbconn, statement_text)
	);
	new_statement->lock();
	if (m_statement_cache.size() != m_cache_capacity)
	{
		assert (m_statement_cache.size() < m_cache_capacity);
		try
		{
			m_statement_cache[statement_text] = new_statement;
		}
		catch (bad_alloc&)
		{
			m_statement_cache.clear();
			assert (new_statement != 0);
		}
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

void
DatabaseConnection::unchecked_begin_transaction()
{
	// JEWEL_DEBUG_LOG << "Beginning SQL transaction." << endl;
	SQLStatement statement(*this, "begin");
	statement.step();
	return;
}

void
DatabaseConnection::unchecked_end_transaction()
{
	// JEWEL_DEBUG_LOG << "Ending SQL transaction." << endl;
	SQLStatement statement(*this, "end");
	statement.step();
	return;
}

void
DatabaseConnection::unchecked_set_savepoint()
{
	// JEWEL_DEBUG_LOG << "Setting SQL savepoint." << endl;
	SQLStatement statement(*this, "savepoint sp");
	statement.step();
	return;
}

void
DatabaseConnection::unchecked_release_savepoint()
{
	// JEWEL_DEBUG_LOG << "Releasing SQL savepoint." << endl;
	SQLStatement statement(*this, "release sp");
	statement.step();
	return;
}

void
DatabaseConnection::unchecked_rollback_transaction()
{
	// JEWEL_DEBUG_LOG << "Rolling back entire SQL transaction." << endl;
	SQLStatement statement(*this, "rollback");
	statement.step();
	return;
}

void
DatabaseConnection::unchecked_rollback_to_savepoint()
{
	// JEWEL_DEBUG_LOG << "Rolling back to SQL savepoint." << endl;
	SQLStatement statement(*this, "rollback to savepoint sp");
	statement.step();
	return;
}

int
DatabaseConnection::self_test()
{
	int ret = 0;
	assert (m_transaction_nesting_level == 0);
	int const original_nesting = m_transaction_nesting_level;
	m_transaction_nesting_level = max_nesting() - 1;
	DatabaseTransaction transaction1(*this);  // Should be ok.
	++ret;
	try
	{
		DatabaseTransaction transaction2(*this);  // Should throw
	}
	catch (TransactionNestingException&)
	{
		--ret;
	}
	transaction1.cancel();
	m_transaction_nesting_level = original_nesting;
	return ret;

}



}  // namespace sqloxx

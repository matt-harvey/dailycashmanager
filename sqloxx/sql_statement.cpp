#include "sql_statement.hpp"
#include "database_connection.hpp"
#include "detail/sql_statement_impl.hpp"
#include <jewel/debug_log.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

using boost::shared_ptr;
using std::endl;  // for debug logging
using std::string;


namespace sqloxx
{



SQLStatement::~SQLStatement()
{
	m_sql_statement->reset();
	m_sql_statement->clear_bindings();
	m_sql_statement->unlock();
}



template <>
int
SQLStatement::extract<int>(int index)
{
	return m_sql_statement->extract<int>(index);
}


template <>
boost::int64_t
SQLStatement::extract<boost::int64_t>(int index)
{
	return m_sql_statement->extract<boost::int64_t>(index);
}


// All these total specialisations of SQLStatement::extract
// are to avoid having to put the call to SQLStatementImpl::extract in the
// header - which would introduce unwanted compilation dependencies.

template <>
double
SQLStatement::extract<double>(int index)
{
	return m_sql_statement->extract<double>(index);
}


template <>
std::string
SQLStatement::extract<std::string>(int index)
{
	return m_sql_statement->extract<std::string>(index);
}


void
SQLStatement::bind(string const& parameter_name, int x)
{
	m_sql_statement->bind(parameter_name, x);
	return;
}


void
SQLStatement::bind(string const& parameter_name, boost::int64_t x)
{
	m_sql_statement->bind(parameter_name, x);
	return;
}


void
SQLStatement::bind(string const& parameter_name, double x)
{
	m_sql_statement->bind(parameter_name, x);
}

void
SQLStatement::bind(string const& parameter_name, string const& x)
{
	m_sql_statement->bind(parameter_name, x);
	return;
}


bool
SQLStatement::step()
{
	return m_sql_statement->step();
}


void
SQLStatement::step_final()
{
	m_sql_statement->step_final();
	return;
}


void
SQLStatement::reset()
{
	m_sql_statement->reset();
	return;
}


void
SQLStatement::clear_bindings()
{
	m_sql_statement->clear_bindings();
	return;
}



}  // namespace sqloxx

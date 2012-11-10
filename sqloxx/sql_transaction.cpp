#include "sql_transaction.hpp"
#include "database_connection.hpp"
#include <iostream>
#include <stdexcept>

using std::cerr;
using std::endl;

namespace sqloxx
{


SQLTransaction::SQLTransaction(DatabaseConnection& dbc):
	m_is_active(false),
	m_database_connection(dbc)
{
	dbc.begin_transaction();
	m_is_active = true;
}


SQLTransaction::~SQLTransaction()
{
	try
	{
		finish();
	}
	catch (std::exception& e)
	{
		try
		{
			cerr << "Exception caught in destructor of SQLTransaction "
			     << "with error message: " << e.what() << endl;
		}
		catch (std::exception&)
		{
		}
	}
}


void
SQLTransaction::finish()
{
	if (m_is_active)
	{
		m_database_connection.end_transaction();
		m_is_active = false;
	}
	return;
}



}  // namespace sqloxx

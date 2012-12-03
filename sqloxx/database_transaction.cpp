#include "database_transaction.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <iostream>
#include <stdexcept>

using std::terminate;
using std::cerr;
using std::endl;
using std::bad_alloc;
using std::exception;

namespace sqloxx
{

DatabaseTransaction::DatabaseTransaction
(	DatabaseConnection& p_database_connection
):
	m_is_active(true),
	m_database_connection(p_database_connection)
{
	m_database_connection.begin_transaction();
}

DatabaseTransaction::~DatabaseTransaction()
{
	if (m_is_active)
	{
		try
		{
			m_database_connection.cancel_transaction();
			m_is_active = false;
		}
		catch (exception& e)
		{
			try
			{
				cerr << "Exception swallowed in destructor of "
				     << "DatabaseTransaction, with error message: "
				     << e.what() << endl
					 << "Calling std::terminate()." << endl;
			}
			catch (bad_alloc&)
			{
			}
			terminate();
		}
	}
}

void
DatabaseTransaction::commit()
{
	if (m_is_active)
	{
		try
		{
			m_database_connection.end_transaction();
		}
		catch (exception&)
		{
			throw UnresolvedTransactionException
			(	"Attempt to commit database transaction has "
				"failed. Transaction remains open. Attempting "
				"further database transactions during this application "
				"session may jeopardize data integrity."
			);
		}
		m_is_active = false;
	}
	else
	{
		throw TransactionNestingException
		(	"Cannot commit inactive SQL transaction."
		);
	}
	return;
}

void
DatabaseTransaction::cancel()
{
	if (m_is_active)
	{
		try
		{
			m_database_connection.cancel_transaction();
		}
		catch (exception&)
		{
			throw UnresolvedTransactionException
			(	"Attempt at formal cancellation of database transaction "
				"has failed. Transaction will still be cancelled back in the"
				"database, but attempting further database transactions "
				"during this application session may jeopardize "
				"this situation."
			);
		}
		m_is_active = false;
	}
	else
	{
		throw TransactionNestingException
		(	"Cannot cancel inactive SQL transaction."
		);
	}
	return;
}


}  // namespace sqloxx

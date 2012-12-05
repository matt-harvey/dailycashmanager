#include "database_transaction.hpp"
#include "database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <cstdio>
#include <iostream>
#include <stdexcept>

using std::terminate;
using std::cerr;
using std::endl;
using std::fprintf;
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
	DatabaseConnection::TransactionAttorney::begin_transaction
	(	m_database_connection
	);
}

DatabaseTransaction::~DatabaseTransaction()
{
	if (m_is_active)
	{
		try
		{
			DatabaseConnection::TransactionAttorney::cancel_transaction
			(	m_database_connection
			);
			m_is_active = false;
		}
		catch (exception& e)
		{
			// We avoid streams here, as, at least in theory, exceptions
			// might be thrown writing to a stream (if exceptions have
			// been enabled for the stream).
			fprintf
			(	stderr,
				"Exception caught in destructor of DatabaseTransaction, "
				"with the error message: %s\n",
				e.what()
			);
			fprintf(stderr, "Program terminated.\n");
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
			DatabaseConnection::TransactionAttorney::end_transaction
			(	m_database_connection
			);
			m_is_active = false;
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
			DatabaseConnection::TransactionAttorney::cancel_transaction
			(	m_database_connection
			);
			m_is_active = false;
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

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_reader.hpp"
#include "account_type.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/reader.hpp>

using jewel::Log;

namespace phatbooks
{


/**
 * TODO The orderings by name here are only guaranteed to work
 * with all-ASCII English text. The ordering for non-English speakers is
 * liable, then, to be meaningless.
 * Here are two possible solutions...
 * 	- (a) Don't order accounts alphabetically at all. Order them by order
 * 	  of creation. This could be done simply by order by the account_id.
 * 	  But then we would also want users to be able to change the ordering.
 * 	  This would require a separate column, containing an ordering number.
 *  - (b) Alternatively, I could rebuild SQLite with ICU support. I could
 *    then do locale-aware lexical ordering using the facilities provided by
 *    the SQLite ICU extension. But even then, I would probably still
 *    want the user to be able to change the order.
 * Solution (a) seems like the best one.
 */

AccountReader::AccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts order by name"
	)
{
}


namespace
{
	void check_account_super_types_boundary(int begin_pl)
	{
#		ifndef NDEBUG
			account_type::AccountType acctype =
				static_cast<account_type::AccountType>(begin_pl - 1);
			JEWEL_ASSERT
			(	super_type(acctype) ==
				account_super_type::balance_sheet
			);

			acctype = static_cast<account_type::AccountType>(begin_pl);
			JEWEL_ASSERT (super_type(acctype) == account_super_type::pl);

			JEWEL_ASSERT
			(	account_super_type::balance_sheet != account_super_type::pl
			);
#		else
  			// Silence compiler warning about unused param.
			(void)begin_pl;
#		endif
		return;
	}
	
	void check_revenue_type_int(int n)
	{
#		ifndef NDEBUG
			int const rev_int = static_cast<int>(account_type::revenue);
			JEWEL_ASSERT (n == rev_int);
#		else
			(void)n;  // Silence compiler re. unused parameter.
#		endif
		return;
	}
		
	void check_expense_type_int(int n)
	{
#		ifndef NDEBUG
			int const exp_int = static_cast<int>(account_type::expense);
			JEWEL_ASSERT (n == exp_int);
#		else
			(void)n;  // Silence compiler re. unused parameter.
#		endif
		return;
	}

	void check_pure_envelope_type(int n)
	{
#		ifndef NDEBUG
			int const pe_int = static_cast<int>(account_type::pure_envelope);
			JEWEL_ASSERT (n == pe_int);
#		else
			(void)n;  // Silence compiler re. unused parameter.
#		endif
		return;
	}
	
}  // end anonymous namespace



/**
 * @todo If we had a table of AccountSuperTypes in the
 * database, this could make this query more robust, and we could get rid of
 * check_account_super_types().
 */
BalanceSheetAccountReader::BalanceSheetAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id <= 3 "
		"order by account_type_id, name"
	)
{
	check_account_super_types_boundary(4);
}


/**
 * @todo If we had a table of AccountSuperTypes in the
 * database, we could make this query more robust, and we could get rid of
 * check_account_super_types().
 */
PLAccountReader::PLAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id >= 4 "
		"order by account_type_id, name"
	)
{
	check_account_super_types_boundary(4);
}


RevenueAccountReader::RevenueAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id = 4 "
		"order by name"
	)
{
	check_revenue_type_int(4);
}

ExpenseAccountReader::ExpenseAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id = 5 "
		"order by name"
	)
{
	check_expense_type_int(5);
}

PureEnvelopeAccountReader::PureEnvelopeAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id = 6 "
		"order by name "
	)
{
	check_pure_envelope_type(6);
}

ImpureAccountReader::ImpureAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id != 6 "
		"order by name "
	)
{
	check_pure_envelope_type(6);
}

AccountReaderBase*
create_source_account_reader
(	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type
)
{
	switch (p_transaction_type)
	{
	case transaction_type::expenditure_transaction:
		return new BalanceSheetAccountReader(p_database_connection);
	case transaction_type::revenue_transaction:
		return new RevenueAccountReader(p_database_connection);
	case transaction_type::balance_sheet_transaction:
		return new BalanceSheetAccountReader(p_database_connection);
	case transaction_type::envelope_transaction:
		return new PLAccountReader(p_database_connection);
	case transaction_type::generic_transaction:
		return new ImpureAccountReader(p_database_connection);
	default:
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_HARD_ASSERT (false);
}

AccountReaderBase*
create_destination_account_reader
(	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type
)
{
	switch (p_transaction_type)
	{
	case transaction_type::expenditure_transaction:
		return new ExpenseAccountReader(p_database_connection);
	case transaction_type::revenue_transaction:
		return new BalanceSheetAccountReader(p_database_connection);
	case transaction_type::balance_sheet_transaction:
		return new BalanceSheetAccountReader(p_database_connection);
	case transaction_type::envelope_transaction:
		return new PLAccountReader(p_database_connection);
	case transaction_type::generic_transaction:
		return new ImpureAccountReader(p_database_connection);
	default:
		JEWEL_LOG_MESSAGE
		(	Log::error,
			"Unexpected TransactionType passed to create_destination_account_reader: "
		);
		JEWEL_LOG_VALUE
		(	Log::error,
			static_cast<int>(p_transaction_type)
		);
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_HARD_ASSERT (false);
}


}  // namespace phatbooks


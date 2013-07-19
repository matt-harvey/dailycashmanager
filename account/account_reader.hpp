// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_reader_hpp
#define GUARD_account_reader_hpp

#include "account.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{


typedef
	sqloxx::Reader<Account, PhatbooksDatabaseConnection>
	AccountReaderBase;


class AccountReader: public AccountReaderBase
{
public:
	explicit AccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


class BalanceSheetAccountReader: public AccountReaderBase
{
public:
	explicit BalanceSheetAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


class PLAccountReader: public AccountReaderBase
{
public:
	explicit PLAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};

class RevenueAccountReader: public AccountReaderBase
{
public:
	explicit RevenueAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};

class ExpenseAccountReader: public AccountReaderBase
{
public:
	explicit ExpenseAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};

class PureEnvelopeAccountReader: public AccountReaderBase
{
public:
	explicit PureEnvelopeAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};

/**
 * Reads only Accounts which are not of account_type::pure_envelope.
 */
class ImpureAccountReader: public AccountReaderBase
{
public:
	explicit ImpureAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


}  // namespace phatbooks

#include "transaction_type.hpp"

namespace phatbooks
{


/**
 * @returns a pointer to a heap-allocated AccountReaderBase,
 * which will read only those Accounts (associated with
 * p_database_connection) which are eligible to be the "source"
 * Account for a transaction of type p_transaction_type.
 *
 * The caller is responsible for managing the memory
 * pointed to by the returned pointer.
 *
 * Guaranteed either to throw an exception, or else return a valid
 * pointer.
 *
 * @todo Is there code/information duplication between here and
 * functions in transaction_type.hpp?
 */
AccountReaderBase* create_source_account_reader
(	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type
);

/**
 * @returns a pointer to a heap-allocated AccountReaderBase,
 * which will read only those Accounts (associated with
 * p_database_connection) which are eligible to be the "destination"
 * Account for a transaction of type p_transaction_type.
 *
 * Guarantees either to throw an exception, or else returns a valid
 * pointer.
 *
 * The caller responsible for managing the memory pointed to by
 * the returned pointer.
 *
 * @todo Is there code/information duplication between here and
 * functions in transaction_type.hpp?
 */
AccountReaderBase* create_destination_account_reader
(	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type
);


}  // namespace phatbooks

#endif  // GUARD_account_reader_hpp

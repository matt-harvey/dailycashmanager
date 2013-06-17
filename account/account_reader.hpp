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



}  // namespace phatbooks

#endif  // GUARD_account_reader_hpp

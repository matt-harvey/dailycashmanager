#ifndef GUARD_account_reader_hpp
#define GUARD_account_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class AccountImpl;
class PhatbooksDatabaseConnection;


typedef
	sqloxx::Reader<AccountImpl, PhatbooksDatabaseConnection>
	AccountReaderBase;


class AccountReader:
	public AccountReaderBase
{
public:
	explicit AccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};



class BalanceSheetAccountReader:
	public AccountReaderBase
{
public:
	explicit BalanceSheetAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};



class PLAccountReader:
	public AccountReaderBase
{
public:
	explicit PLAccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};



}  // namespace phatbooks

#endif  // GUARD_account_reader_hpp

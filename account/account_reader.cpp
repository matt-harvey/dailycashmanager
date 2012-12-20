#include "account_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{

AccountReader::AccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts order by name"
	)
{
}


BalanceSheetAccountReader::BalanceSheetAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id <= 3 "
		"order by account_type_id, name"
	)
{
}


PLAccountReader::PLAccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	AccountReaderBase
	(	p_database_connection,
		"select account_id from accounts where account_type_id >= 4 "
		"order by account_type_id, name"
	)
{
}




}  // namespace phatbooks


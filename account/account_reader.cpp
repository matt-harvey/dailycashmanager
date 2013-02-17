#include "account_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

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


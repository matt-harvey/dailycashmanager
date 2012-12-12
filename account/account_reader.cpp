#include "account_impl.hpp"
#include "account_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/reader.hpp"

namespace phatbooks
{


AccountReader::AccountReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<AccountImpl, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select account_id from accounts order by account_type_id, "
		"name"
	)
{
}






}  // namespace phatbooks


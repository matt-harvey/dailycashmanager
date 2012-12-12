#include "phatbooks_database_connection.hpp"
#include "repeater_impl.hpp"
#include "repeater_reader.hpp"
#include "sqloxx/reader.hpp"

namespace phatbooks
{

RepeaterReader::RepeaterReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<RepeaterImpl, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select repeater_id from repeaters order by date"
	)
{
}




}  // namespace phatbooks

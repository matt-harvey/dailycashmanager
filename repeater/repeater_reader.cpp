#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "repeater_reader.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{

RepeaterReader::RepeaterReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<Repeater, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select repeater_id from repeaters order by date"
	)
{
}




}  // namespace phatbooks

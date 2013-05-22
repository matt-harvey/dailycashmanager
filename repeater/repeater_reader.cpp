// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "repeater_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include <sqloxx/reader.hpp>

using sqloxx::Reader;

namespace phatbooks
{

RepeaterReader::RepeaterReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	Reader<Repeater, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select repeater_id from repeaters"
	)
{
}




}  // namespace phatbooks

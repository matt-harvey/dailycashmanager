// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_repeater_reader_hpp
#define GUARD_repeater_reader_hpp

#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{


class RepeaterReader:
	public sqloxx::Reader<Repeater, PhatbooksDatabaseConnection>
{
public:
	RepeaterReader(PhatbooksDatabaseConnection& p_database_connection);
};


}  // namespace phatbooks

#endif  // GUARD_repeater_reader_hpp

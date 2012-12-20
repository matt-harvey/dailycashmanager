#ifndef GUARD_commodity_reader_hpp
#define GUARD_commodity_reader_hpp

#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{


typedef
	sqloxx::Reader<Commodity, PhatbooksDatabaseConnection>
	CommodityReader;



}  // namespace phatbooks

#endif  // GUARD_commodity_reader_hpp

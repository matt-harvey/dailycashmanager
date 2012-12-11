#ifndef GUARD_commodity_reader_hpp
#define GUARD_commodity_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class CommodityImpl;
class PhatbooksDatabaseConnection;

typedef
	sqloxx::Reader<CommodityImpl, PhatbooksDatabaseConnection>
	CommodityReader;



}  // namespace phatbooks

#endif  // GUARD_commodity_reader_hpp

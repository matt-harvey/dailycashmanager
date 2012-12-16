#ifndef GUARD_repeater_reader_hpp
#define GUARD_repeater_reader_hpp

#include "phatbooks_database_connection.hpp"
#include "repeater_impl.hpp"
#include "sqloxx/reader.hpp"

namespace phatbooks
{


class RepeaterReader:
	public sqloxx::Reader<RepeaterImpl, PhatbooksDatabaseConnection>
{
public:
	RepeaterReader(PhatbooksDatabaseConnection& p_database_connection);
};


}  // namespace phatbooks

#endif  // GUARD_repeater_reader_hpp

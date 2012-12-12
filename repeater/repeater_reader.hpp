#ifndef GUARD_repeater_reader_hpp
#define GUARD_repeater_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class RepeaterImpl;
class PhatbooksDatabaseConnection;


class RepeaterReader:
	public sqloxx::Reader<RepeaterImpl, PhatbooksDatabaseConnection>
{
public:
	RepeaterReader(PhatbooksDatabaseConnection& p_database_connection);
};


}  // namespace phatbooks

#endif  // GUARD_repeater_reader_hpp

#ifndef GUARD_repeater_reader_hpp
#define GUARD_repeater_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class RepeaterImpl;
class PhatbooksDatabaseConnection;

typedef
	sqloxx::Reader<RepeaterImpl, PhatbooksDatabaseConnection>
	RepeaterReader;


}


#endif  // GUARD_repeater_reader_hpp

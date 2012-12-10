#ifndef GUARD_ordinary_entry_reader_hpp
#define GUARD_ordinary_entry_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class EntryImpl;
class PhatbooksDatabaseConnection;


class OrdinaryEntryReader:
	public sqloxx::Reader<EntryImpl, PhatbooksDatabaseConnection>
{
public:
	explicit OrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};




}  // namespace phatbooks


#endif  // GUARD_ordinary_entry_reader_hpp

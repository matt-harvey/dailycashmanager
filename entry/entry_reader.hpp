#ifndef GUARD_entry_reader_hpp
#define GUARD_entry_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class EntryImpl;
class PhatbooksDatabaseConnection;


typedef
	sqloxx::Reader<EntryImpl, PhatbooksDatabaseConnection>
	EntryReader;


class OrdinaryEntryReader:
	public EntryReader
{
public:
	explicit OrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};




}  // namespace phatbooks


#endif  // GUARD_entry_reader_hpp

#ifndef GUARD_entry_reader_hpp
#define GUARD_entry_reader_hpp

#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{



typedef
	sqloxx::Reader<Entry, PhatbooksDatabaseConnection>
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

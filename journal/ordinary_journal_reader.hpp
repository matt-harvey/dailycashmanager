#ifndef GUARD_ordinary_journal_reader_hpp
#define GUARD_ordinary_journal_reader_hpp

#include "ordinary_journal_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/reader.hpp"

namespace phatbooks
{


class OrdinaryJournalReader:
	public sqloxx::Reader<OrdinaryJournalImpl, PhatbooksDatabaseConnection>
{
public:
	explicit OrdinaryJournalReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


}  // namespace phatbooks

#endif  // GUARD_ordinary_journal_reader_hpp

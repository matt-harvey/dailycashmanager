#ifndef GUARD_draft_journal_reader_hpp
#define GUARD_draft_journal_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class DraftJournalImpl;
class PhatbooksDatabaseConnection;

class DraftJournalReader:
	public sqloxx::Reader<DraftJournalImpl, PhatbooksDatabaseConnection>
{
public:
	explicit DraftJournalReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};



}  // namespace phatbooks

#endif  // GUARD_draft_journal_reader_hpp

// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_reader_hpp
#define GUARD_draft_journal_reader_hpp

#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{


/**
 * Reads all DraftJournals including "system" DraftJournals which it is
 * not intended for the user to edit directly.
 */
class DraftJournalReader:
	public sqloxx::Reader<DraftJournal, PhatbooksDatabaseConnection>
{
public:
	explicit DraftJournalReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


/**
 * Reads all DraftJournals except "system" DraftJournals which it is not
 * intended for the user to edit directly.
 */
class UserDraftJournalReader:
	public sqloxx::Reader<DraftJournal, PhatbooksDatabaseConnection>
{
public:
	explicit UserDraftJournalReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


}  // namespace phatbooks

#endif  // GUARD_draft_journal_reader_hpp

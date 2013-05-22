// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_reader_hpp
#define GUARD_draft_journal_reader_hpp

#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{


class DraftJournalReader:
	public sqloxx::Reader<DraftJournal, PhatbooksDatabaseConnection>
{
public:
	explicit DraftJournalReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};


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

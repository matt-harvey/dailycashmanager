// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_table_iterator_hpp
#define GUARD_draft_journal_table_iterator_hpp

#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{


typedef
	sqloxx::TableIterator<DraftJournal, PhatbooksDatabaseConnection>
	DraftJournalTableIterator;

/**
 * Reads all DraftJournals except "system" DraftJournals which it is not
 * intended for the user to edit directly. Orders the results by name.
 */
class UserDraftJournalTableIterator: public DraftJournalTableIterator
{
public:
	UserDraftJournalTableIterator();
	explicit UserDraftJournalTableIterator
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};



}  // namespace phatbooks

#endif  // GUARD_draft_journal_table_iterator_hpp

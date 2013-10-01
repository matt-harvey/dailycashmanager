// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_table_iterator_hpp_8254837704941089
#define GUARD_draft_journal_table_iterator_hpp_8254837704941089

#include "draft_journal_handle.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{


typedef
	sqloxx::TableIterator<DraftJournalHandle, PhatbooksDatabaseConnection>
	DraftJournalTableIterator;


/**
 * @returns DraftJournalTableIterator that reads all DraftJournals in
 * order by name, except those which are "system" DraftJournal which
 * are not intended for the user to edit directly.
 */
DraftJournalTableIterator
make_name_ordered_user_draft_journal_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);



}  // namespace phatbooks

#endif  // GUARD_draft_journal_table_iterator_hpp_8254837704941089

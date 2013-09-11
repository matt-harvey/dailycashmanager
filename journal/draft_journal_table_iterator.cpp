// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "draft_journal_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"


namespace phatbooks
{


UserDraftJournalTableIterator::UserDraftJournalTableIterator
(	PhatbooksDatabaseConnection& p_database_connection
):
	DraftJournalTableIterator
	(	p_database_connection,
		"select journal_id from draft_journal_detail "
		"left join amalgamated_budget_data using(journal_id) "
		"where amalgamated_budget_data.journal_id is null "
		"order by name"
	)
{
}

UserDraftJournalTableIterator::UserDraftJournalTableIterator()
{
}



}  // namespace phatbooks

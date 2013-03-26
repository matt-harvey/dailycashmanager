#include "draft_journal_reader.hpp"
#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>


namespace phatbooks
{


DraftJournalReader::DraftJournalReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<DraftJournal, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select journal_id from draft_journal_detail order by name"
	)
{
}


UserDraftJournalReader::UserDraftJournalReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<DraftJournal, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select journal_id from draft_journal_detail "
		"left join amalgamated_budget_data using(journal_id) "
		"where amalgamated_budget_data.journal_id is null "
		"order by name"
	)
{
}



}  // namespace phatbooks

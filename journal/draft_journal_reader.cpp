#include "draft_journal_reader.hpp"
#include "draft_journal_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/reader.hpp"


namespace phatbooks
{


DraftJournalReader::DraftJournalReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<DraftJournalImpl, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select journal_id from draft_journal_detail"
	)
{
}

}  // namespace phatbooks

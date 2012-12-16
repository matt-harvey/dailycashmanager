#include "ordinary_journal_reader.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/reader.hpp"

namespace phatbooks
{


OrdinaryJournalReader::OrdinaryJournalReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<OrdinaryJournal, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select journal_id from ordinary_journal_detail order by date"
	)
{
}






}  // namespace phatbooks

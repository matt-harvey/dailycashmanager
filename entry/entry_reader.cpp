#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"


namespace phatbooks
{

OrdinaryEntryReader::OrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	EntryReader
	(	p_database_connection,
		"select entry_id from entries inner join ordinary_journal_detail "
		"using(journal_id) order by date"
	)
{
}

ActualOrdinaryEntryReader::ActualOrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	EntryReader
	(	p_database_connection,
		"select entry_id from entries inner join ordinary_journal_detail "
		"using(journal_id) join journals using(journal_id) "
		"where is_actual = 1 order by date"
	)
{
}

}  // namespace phatbooks

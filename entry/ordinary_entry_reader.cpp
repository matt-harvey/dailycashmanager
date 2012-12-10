#include "ordinary_entry_reader.hpp"
#include "entry_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/reader.hpp"


namespace phatbooks
{

OrdinaryEntryReader::OrdinaryEntryReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	sqloxx::Reader<EntryImpl, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select entry_id from entries inner join ordinary_journal_detail "
		"using(journal_id)"
	)
{
}





}  // namespace phatbooks

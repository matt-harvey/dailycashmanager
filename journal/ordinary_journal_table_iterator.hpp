#ifndef GUARD_ordinary_journal_table_iterator_hpp
#define GUARD_ordinary_journal_table_iterator_hpp

#include "ordinary_journal.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<OrdinaryJournal, PhatbooksDatabaseConnection>
	OrdinaryJournalTableIterator;


}  // namespace phatbooks

#endif  // GUARD_ordinary_journal_table_iterator_hpp

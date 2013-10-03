// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_ordinary_journal_table_iterator_hpp_9057306944884969
#define GUARD_ordinary_journal_table_iterator_hpp_9057306944884969

#include "ordinary_journal_handle.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<OrdinaryJournalHandle>
	OrdinaryJournalTableIterator;


}  // namespace phatbooks

#endif  // GUARD_ordinary_journal_table_iterator_hpp_9057306944884969

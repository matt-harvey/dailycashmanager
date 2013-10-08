// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_table_iterator_hpp_09390961802280956
#define GUARD_entry_table_iterator_hpp_09390961802280956

#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<sqloxx::Handle<Entry> >
	EntryTableIterator;

/**
 * Returns an EntryTableIterator that reads in date ordered all and only
 * the actual (i.e. non-budget), ordinary (i.e. associated with
 * OrdinaryJournals not DraftJournals) Entries in the database.
 */
EntryTableIterator
make_date_ordered_actual_ordinary_entry_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

}  // namespace phatbooks

#endif  // GUARD_entry_table_iterator_hpp_09390961802280956

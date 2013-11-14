/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

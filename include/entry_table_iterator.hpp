/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

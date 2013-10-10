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


#ifndef GUARD_draft_journal_table_iterator_hpp_8254837704941089
#define GUARD_draft_journal_table_iterator_hpp_8254837704941089

#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{


typedef
	sqloxx::TableIterator<sqloxx::Handle<DraftJournal> >
	DraftJournalTableIterator;


/**
 * @returns DraftJournalTableIterator that reads all DraftJournals in
 * order by name, except those which are "system" DraftJournal which
 * are not intended for the user to edit directly.
 */
DraftJournalTableIterator
make_name_ordered_user_draft_journal_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);



}  // namespace phatbooks

#endif  // GUARD_draft_journal_table_iterator_hpp_8254837704941089

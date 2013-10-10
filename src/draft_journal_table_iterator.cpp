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


#include "draft_journal_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"


namespace phatbooks
{


DraftJournalTableIterator
make_name_ordered_user_draft_journal_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return DraftJournalTableIterator
	(	p_database_connection,
		"select journal_id from draft_journal_detail "
		"left join amalgamated_budget_data using(journal_id) "
		"where amalgamated_budget_data.journal_id is null "
		"order by name"
	);
}



}  // namespace phatbooks

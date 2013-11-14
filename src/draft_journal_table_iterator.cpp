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

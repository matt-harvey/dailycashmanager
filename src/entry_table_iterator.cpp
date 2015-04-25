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

#include "entry_table_iterator.hpp"
#include "dcm_database_connection.hpp"
#include "transaction_type.hpp"
#include <jewel/assert.hpp>
#include <sstream>

using std::ostringstream;

namespace dcm
{

EntryTableIterator
make_date_ordered_actual_ordinary_entry_table_iterator
(   DcmDatabaseConnection& p_database_connection
)
{
    ostringstream oss;
    oss << "select entry_id from entries inner join ordinary_journal_detail "
        << "using(journal_id) join journals using(journal_id) "
        << "where transaction_type_id != "
        << static_cast<int>(non_actual_transaction_type())
        << " order by date";
    return EntryTableIterator
    (   p_database_connection,
        oss.str()
    );
}


}  // namespace dcm

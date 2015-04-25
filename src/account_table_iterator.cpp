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

#include "account_table_iterator.hpp"

namespace dcm
{

AccountTableIterator
make_name_ordered_account_table_iterator
(   DcmDatabaseConnection& p_database_connection
)
{
    return AccountTableIterator
    (   p_database_connection,
        "select account_id from accounts order by name"
    );
}

AccountTableIterator
make_type_name_ordered_account_table_iterator
(   DcmDatabaseConnection& p_database_connection
)
{
    return AccountTableIterator
    (   p_database_connection,
        "select account_id from accounts order by account_type_id, name"
    );
}





}  // namespace dcm

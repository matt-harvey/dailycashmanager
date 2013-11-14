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

#ifndef GUARD_account_table_iterator_hpp_048275898233184546
#define GUARD_account_table_iterator_hpp_048275898233184546

#include "account.hpp"
#include <sqloxx/handle.hpp>
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<sqloxx::Handle<Account> >
	AccountTableIterator;

/**
 * @returns an AccountTableIterator that iterates over all Accounts
 * ordered by name.
 */
AccountTableIterator
make_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

/**
 * @returns an AccountTableIterator that iterates over all Accounts,
 * ordered first by AccountType, then by name.
 */
AccountTableIterator
make_type_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

}  // namespace phatbooks

#endif  // GUARD_account_table_iterator_hpp_048275898233184546

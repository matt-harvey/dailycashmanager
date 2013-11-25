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

#ifndef GUARD_budget_item_table_iterator_hpp_017771196094235032
#define GUARD_budget_item_table_iterator_hpp_017771196094235032

#include "budget_item.hpp"
#include "dcm_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <sqloxx/table_iterator.hpp>

namespace dcm
{

typedef 
	sqloxx::TableIterator<sqloxx::Handle<BudgetItem> >
	BudgetItemTableIterator;

}  // namespace dcm

#endif  // GUARD_budget_item_table_iterator_hpp_017771196094235032

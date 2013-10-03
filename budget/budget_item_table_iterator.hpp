// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_table_iterator_hpp_017771196094235032
#define GUARD_budget_item_table_iterator_hpp_017771196094235032

#include "budget_item_handle.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef 
	sqloxx::TableIterator<BudgetItemHandle>
	BudgetItemTableIterator;

}  // namespace phatbooks

#endif  // GUARD_budget_item_table_iterator_hpp_017771196094235032

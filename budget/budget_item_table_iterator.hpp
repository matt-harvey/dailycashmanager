// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_table_iterator_hpp
#define GUARD_budget_item_table_iterator_hpp

#include "budget_item.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef 
	sqloxx::TableIterator<BudgetItem, PhatbooksDatabaseConnection>
	BudgetItemTableIterator;

}  // namespace phatbooks

#endif  // GUARD_budget_item_table_iterator_hpp

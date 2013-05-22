// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_budget_item_reader_hpp
#define GUARD_budget_item_reader_hpp

#include "budget_item.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{


class BudgetItemReader:
	public sqloxx::Reader<BudgetItem, PhatbooksDatabaseConnection>
{
public:
	BudgetItemReader(PhatbooksDatabaseConnection& p_database_connection);
};

}  // namespace phatbooks

#endif  // GUARD_budget_item_reader_hpp

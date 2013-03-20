#include "budget_item_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "budget_item.hpp"
#include <sqloxx/reader.hpp>

using sqloxx::Reader;

namespace phatbooks
{

BudgetItemReader::BudgetItemReader
(	PhatbooksDatabaseConnection& p_database_connection
):
	Reader<BudgetItem, PhatbooksDatabaseConnection>
	(	p_database_connection,
		"select budget_item_id from budget_items"
	)
{
}


}  // namespace phatbooks

#include "budget_manager.hpp"
#include "phatbooks_database_connection.hpp"

namespace phatbooks
{

BudgetManager::BudgetManager
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection),
	m_map(new Map)
{
}



}  // namespace phatbooks

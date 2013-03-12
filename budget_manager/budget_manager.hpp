#ifndef GUARD_budget_manager_hpp
#define GUARD_budget_manager_hpp

#include "account.hpp"
#include "draft_journal.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>


namespace phatbooks
{

// Begin forward declarations
class BudgetData;
class PhatbooksDatabaseConnection;
// End forward declarations



class BudgetManager:
	public boost::noncopyable
{
public:
	PhatbooksDatabaseConnection& database_connection() const;
	DraftJournal posting_instrument() const;
	
	

private:
	typedef boost::unordered_map<Account::Id, BudgetData> Map;
	PhatbooksDatabaseConnection& m_database_connection;
	boost::scoped_ptr<Map> m_map;
};


}  // namespace phatbooks

#endif  // GUARD_budget_manager_hpp

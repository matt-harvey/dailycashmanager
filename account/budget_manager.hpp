#ifndef GUARD_budget_manager_hpp
#define GUARD_budget_manager_hpp

#include "account.hpp"
#include "budget_item.hpp"
#include "draft_journal.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>


// Begin forward declarations
namespace jewel
{
	class Decimal;
}

namespace phatbooks
{

class PhatbooksDatabaseConnection;
// End forward declarations



/**
 * @todo HIGH PRIORITY Implement this stuff!
 */
class BudgetManager:
	public boost::noncopyable
{
public:

	
	BudgetManager(PhatbooksDatabaseConnection& m_database_connection);
	
	/**
	 * The "instrument" is a recurring DraftJournal that happens
	 * to reflect the budgets stored in the BudgetManager. The
	 * instrument is posted at some generally infrequent interval - for
	 * example, every year. More "precisely timed" envelope balances
	 * must be calculated with the help of request_update_to (see below).
	 *
	 * @todo We have to make sure that interim envelope balances are
	 * not exposed to the user, as they will not be valid. The running
	 * balance concept applies only to balance sheet Accounts. Is this
	 * further reason to make subclasses PLAccount and BSAccount, to
	 * "hard wire" a mechanism for preventing the exposure of
	 * invalid interim balances?
	 *
	 * @todo When the instrument autoposts, it needs to let the
	 * BalanceCache know. Or else, the BalanceCache will need some other
	 * way to find out when the last posting date of a given journal
	 * actually way. DraftJournal objects currently only store the NEXT
	 * posting date. It may be easiest to simply add this last posting
	 * date to Repeater as another data member. Then the BudgetManager
	 * can simply query the Repeater via the DraftJournal to find
	 * the last posting date.
	 * It needs this information for when it executes
	 * hypothetical_update(...) and permanent_update(...). But note
	 * if it posts its own update journal later than the instrument does
	 * its posting, then this must become the "last update date".
	 */
	DraftJournal instrument() const;

	/**
	 * When the BalanceCache requires a balance, it asks the BudgetManager
	 * to populate a map from Account::Id to jewel::Decimal. It passes
	 * the BudgetManager this map by reference. For each Account for which
	 * the BudgetManager stores a budget (regardless of whether
	 * that Account appears in the passed map), it adds to the
	 * jewel::Decimal for that Account in the map (inserting if
	 * not already present) the additional amount
	 * required to bring the envelope balance up to date, relative to
	 * what has so far been posted.
	 * The map is like a form, which the BalanceCache is asking the
	 * BudgetManager to fill in.
	 */
	void hypothetical_update
	(	boost::unordered_map
		<	Account::Id,
			boost::optional<jewel::Decimal>
		>& p_form,
		boost::gregorian::date const& p_date
	);

	/**
	 * This is like request_hypothetical_update(...), except that instead of
	 * filling in a "form", the BudgetManager is asked actually to
	 * post a journal that will bring the budget up to date. This should
	 * happen any time the user changes a budget.
	 *
	 * @todo Make sure this doesn't conflict with the posting that
	 * is otherwise automatically done by the DraftJournal
	 * returned by instrument().
	 */
	void permanent_update
	(	boost::gregorian::date const& p_date
	);


private:
	typedef boost::unordered_map<Account::Id, BudgetItem> Map;
	PhatbooksDatabaseConnection& m_database_connection;
	boost::scoped_ptr<Map> m_map;

	// WARNING This probably shouldn't be an optional.
	boost::optional<boost::gregorian::date> m_last_permanent_update;
};


}  // namespace phatbooks

#endif  // GUARD_budget_manager_hpp

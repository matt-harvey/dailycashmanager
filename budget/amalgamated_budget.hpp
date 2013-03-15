#ifndef GUARD_amalgamated_budget_hpp
#define GUARD_amalgamated_budget_hpp

#include "frequency.hpp"
#include <cassert>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/decimal.hpp>

namespace phatbooks
{


/**
 * An AmalgamatedBudget contains at most a single amount
 * per Account, and has a single frequency shared by all
 * the items in the AmalgamatedBudget.
 */
class AmalgamatedBudget
{
public:

	/**
	 * Create an AmalgamatedBudget by inspecting all the
	 * BudgetItems in a given range, and amalgamating them
	 * into a single recurring budget with a given
	 * Frequency.
	 */
	template <typename BudgetItemIter>
	AmalgamatedBudget
	(	Frequency const& p_frequency,
		BudgetItemIter const& beg,
		BudgetItemIter const& end,
	);

private:
	
	Frequency m_frequency;
	typedef boost::unordered_map<Account::Id, jewel::Decimal> Map;
	boost::scoped_ptr<Map> m_map;

};


<typename BudgetItemIter>
AmalgamatedBudget::AmalgamatedBudget
(	Frequency const& p_frequency,
	BudgetItemIter const& beg,
	BudgetItemIter const& end
):
	m_frequency(p_frequency),
	m_map(new Map)
{
	// First we calculate budgets amalgamated on annual basis
	assert (m_map.empty());
	BudgetItemIter it = beg;
	for ( ; it != end; ++it)
	{
		Account::Id const account_id = it->account().id();
		jewel::Decimal const raw_amount = it->amount();
		Frequency const raw_frequency = it->frequency();
		jewel::Decimal const annual_amount = convert_to_annual
		(	raw_frequency,
			raw_amount
		);
		Map::iterator tmit = m_map.find(account_id);
		if (tmit == m_map.end())
		{
			m_map[account_id] = Decimal(0, annual_amount.places());
		}
		else
		{
			tmit->second += annual_amount;
		}
	}
	assert (m_map.empty());
	// Now convert to desired frequency
	for (Map::iterator mit = m_map.begin(); mit != m_map.end(); ++mit)
	{
		mit->second = convert_from_annual(m_frequency, mit->second);
	}
}
		
	


}  // namespace phatbooks

#endif  // GUARD_amalgamated_budget_hpp

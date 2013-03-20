#include "amalgamated_budget.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "budget_item_reader.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include <boost/scoped_ptr.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <algorithm>
#include <cassert>
#include <vector>

using boost::scoped_ptr;
using jewel::Decimal;
using std::vector;

// For debugging only
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff

// TODO Sort out whether, conceptually, we are looking at
// Account::Id, or at AccountImpl::Id. Amend code here to reflect
// (currently it is confused in regards to this). Make sure the header
// reflects this too. (In practice it doesn't matter though.)

namespace phatbooks
{

void
AmalgamatedBudget::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create index budget_item_account_index on budget_items(account_id)"
	);
	return;
}

AmalgamatedBudget::AmalgamatedBudget
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection),
	m_frequency(1, interval_type::days),
	m_map(new Map),
	m_map_is_stale(true)
{
}

Frequency
AmalgamatedBudget::frequency() const
{
	return m_frequency;
}


void
AmalgamatedBudget::set_frequency(Frequency const& p_frequency)
{
	mark_as_stale();
	m_frequency = p_frequency;
	return;
}



Decimal
AmalgamatedBudget::budget(AccountImpl::Id p_account_id)
{
	if (m_map_is_stale)
	{
		refresh();
	}
	Map::const_iterator it = m_map->find(p_account_id);
	assert (it != m_map->end());
	return it->second;
}


// TODO Make sure I mark as stale whenever required.
void
AmalgamatedBudget::mark_as_stale()
{
	m_map_is_stale = true;
}

void
AmalgamatedBudget::generate_supported_frequencies(vector<Frequency>& vec)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supports_frequency. If this
	// changes, that must change too.
	vec.push_back(Frequency(1, interval_type::days));
	vec.push_back(Frequency(1, interval_type::weeks));
	vec.push_back(Frequency(2, interval_type::weeks));
	vec.push_back(Frequency(4, interval_type::weeks));
	vec.push_back(Frequency(1, interval_type::months));
	vec.push_back(Frequency(2, interval_type::months));
	vec.push_back(Frequency(3, interval_type::months));
	vec.push_back(Frequency(4, interval_type::months));
	vec.push_back(Frequency(6, interval_type::months));
	vec.push_back(Frequency(12, interval_type::months));
	return;
}


bool
AmalgamatedBudget::supports_frequency(Frequency const& p_frequency)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supported_frequencies. If this changes,
	// that must change too.
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_frequency.num_steps() == 1;
	case interval_type::weeks:
		switch (p_frequency.num_steps())
		{
		case 1: case 2:	return true;
		case 3:			return false;
		case 4:			return true;
		default: 		return false;
		}
	case interval_type::months:
		switch (p_frequency.num_steps())
		{
		case 1: case 2: case 3: case 4:				return true;
		case 5:										return false;
		case 6:										return true;
		case 7: case 8: case 9: case 10: case 11:	return false;
		case 12:									return true;	
		default:									return false;
		}
	case interval_type::month_ends:
		return false;
	default:
		assert (false);
	}
}
		
void
AmalgamatedBudget::refresh()
{
	JEWEL_DEBUG_LOG << "Refreshing AmalgamatedBudget." << endl;
	scoped_ptr<Map> map_elect(new Map);
	assert (map_elect->empty());

	AccountReader const account_reader(m_database_connection);
	for
	(	AccountReader::const_iterator it = account_reader.begin(),
			end = account_reader.end();
		it != end;
		++it
	)
	{
		(*map_elect)[it->id()] = Decimal(0, it->commodity().precision());
	}

	BudgetItemReader budget_item_reader(m_database_connection);
	BudgetItemReader::const_iterator const beg = budget_item_reader.begin();
	BudgetItemReader::const_iterator const end = budget_item_reader.end();
	
	// First we calculate budgets amalgamated on the basis of
	// the canonical frequency
	BudgetItemReader::const_iterator it = beg;
	for ( ; it != end; ++it)
	{
		Frequency const raw_frequency = it->frequency();
		if (!AmalgamatedBudget::supports_frequency(raw_frequency))
		{
			throw InvalidFrequencyException
			(	"Frequency not supported by AmalgamatedBudget."
			);
		}
		Account::Id const account_id = it->account().id();
		jewel::Decimal const raw_amount = it->amount();
		jewel::Decimal const canonical_amount = convert_to_canonical
		(	raw_frequency,
			raw_amount
		);
		Map::iterator tmit = map_elect->find(account_id);
		assert (tmit != map_elect->end());	
		tmit->second += canonical_amount;
	}
	// Now convert to desired frequency
	for 
	(	Map::iterator mit = map_elect->begin(), mend = map_elect->end();
		mit != mend;
		++mit
	)
	{
		mit->second = convert_from_canonical(m_frequency, mit->second);
	}
	using std::swap;
	swap(m_map, map_elect);
	return;
}
		
	


}  // namespace phatbooks



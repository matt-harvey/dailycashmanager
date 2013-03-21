#ifndef GUARD_amalgamated_budget_hpp
#define GUARD_amalgamated_budget_hpp

#include "account_impl.hpp"
#include "interval_type.hpp"
#include "frequency.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/decimal.hpp>
#include <vector>




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

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	AmalgamatedBudget(PhatbooksDatabaseConnection& p_database_connection);

	/**
	 * @returns the Frequency of the AmalgamatedBudget. This
	 * is guaranteed to be always such that
	 * num_steps() == 1.
	 */
	Frequency frequency() const;

	void set_frequency(Frequency const& p_frequency);

	/**
	 * @returns the amalgamated budget for the given Account, at the
	 * Frequency of the AmalgamatedBudget.
	 */
	jewel::Decimal budget(AccountImpl::Id p_account_id);

	/**
	 * Mark the AmalgamatedBudget as a whole as stale.
	 */
	void mark_as_stale();

	/**
	 * Populates vec with all and only the Frequencies that are supported
	 * by AmalgamatedBudget, in an order from smallest to largest.
	 */
	static void generate_supported_frequencies
	(	std::vector<Frequency>& vec
	);

	static bool supports_frequency(Frequency const& p_frequency);

private:

	typedef boost::unordered_map<AccountImpl::Id, jewel::Decimal> Map;
	void refresh();

	PhatbooksDatabaseConnection& m_database_connection;
	Frequency m_frequency;
	boost::scoped_ptr<Map> m_map;
	bool m_map_is_stale;

};






}  // namespace phatbooks

#endif  // GUARD_amalgamated_budget_hpp

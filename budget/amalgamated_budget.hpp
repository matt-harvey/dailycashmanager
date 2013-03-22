#ifndef GUARD_amalgamated_budget_hpp
#define GUARD_amalgamated_budget_hpp

#include "account_impl.hpp"
#include "draft_journal.hpp"
#include "interval_type.hpp"
#include "frequency.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/decimal.hpp>
#include <ostream>
#include <vector>




namespace phatbooks
{


/**
 * An AmalgamatedBudget contains at most a single amount
 * per Account, and has a single frequency shared by all
 * the items in the AmalgamatedBudget.
 */
class AmalgamatedBudget:
	boost::noncopyable
{
public:

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	AmalgamatedBudget(PhatbooksDatabaseConnection& p_database_connection);

	~AmalgamatedBudget();

	/**
	 * @returns the Frequency of the AmalgamatedBudget. This
	 * is guaranteed to be always such that
	 * num_steps() == 1.
	 */
	Frequency frequency() const;

	void set_frequency(Frequency const& p_frequency);

	/**
	 * @returns the amalgamated budget for the given Account, at the
	 * Frequency returned by frequency().
	 */
	jewel::Decimal budget(AccountImpl::Id p_account_id);

	/**
	 * @returns the sum of all the Account budgets in AmalgamatedBudget,
	 * at the Frequency returned by frequency().
	 */
	jewel::Decimal balance() const;

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
	void refresh_instrument();
	void load_instrument();

	/**
	 * Create and push Entries onto \e journal that reflect the
	 * AmalgamatedBudget. This does nothing to change the Repeaters,
	 * comment or other journal-level attributes of journal - it only
	 * affects the entries. If there are already Entries in journal,
	 * these are all cleared prior to the new Entries being pushed on.
	 */
	void reflect_entries(DraftJournal& journal);

	/**
	 * Examines the Repeaters of \e journal. If there is exactly one
	 * Repeater, and it reflects the same Frequency as the AmalgamatedBudget
	 * Frequency, then that Repeater is left unchanged. Otherwise, all
	 * Repeaters are cleared from \e journal, and a new Repeater is
	 * pushed onto \e journal, with TODAY as its next_date().
	 */
	void reflect_repeater(DraftJournal& journal);

	PhatbooksDatabaseConnection& m_database_connection;
	Frequency m_frequency;
	boost::scoped_ptr<Map> m_map;
	bool m_map_is_stale;

	// The DraftJournal that "effects" the AmalgamatedBudget
	DraftJournal* m_instrument; 


};





}  // namespace phatbooks

#endif  // GUARD_amalgamated_budget_hpp

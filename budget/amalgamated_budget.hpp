// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_amalgamated_budget_hpp_5997503626159161
#define GUARD_amalgamated_budget_hpp_5997503626159161

#include "account_impl.hpp"
#include "draft_journal.hpp"
#include "interval_type.hpp"
#include "frequency.hpp"
#include "phatbooks_exceptions.hpp"
#include <jewel/decimal.hpp>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>




namespace phatbooks
{


class Account;


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

	AmalgamatedBudget(AmalgamatedBudget const&) = delete;
	AmalgamatedBudget(AmalgamatedBudget&&) = delete;
	AmalgamatedBudget& operator=(AmalgamatedBudget const&) = delete;
	AmalgamatedBudget& operator=(AmalgamatedBudget&&) = delete;

	~AmalgamatedBudget() = default;

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
	jewel::Decimal budget(AccountImpl::Id p_account_id) const;

	/**
	 * @returns the sum of all the Account budgets in AmalgamatedBudget,
	 * at the Frequency returned by frequency().
	 *
	 * Note this is non-const only because it requires calling
	 * load().
	 */
	jewel::Decimal balance() const;

	/**
	 * Regenerate the AmalgamatedBudget on the basis of the currently
	 * saved BudgetItems.
	 */
	void regenerate();

	/**
	 * Populates vec with all and only the Frequencies that are supported
	 * by AmalgamatedBudget, in an order from smallest to largest.
	 */
	static void generate_supported_frequencies
	(	std::vector<Frequency>& vec
	);

	static bool supports_frequency(Frequency const& p_frequency);

	/**
	 * @returns the Account such, that if the AmalgamatedBudget
	 * is not otherwise "balanced", any imbalanced is reconciled
	 * to this Account.
	 *
	 * @throws UninitializedBalancingAccountException if
	 * balancing account is uninitialized.
	 */
	Account balancing_account() const;

	/**
	 * @returns the DraftJournal that serves as the "instrument"
	 * by means of which the AmalgamatedBudget effects regular
	 * distributions of budget amounts to budgeting envelopes
	 * (Accounts).
	 */
	DraftJournal instrument() const;


private:

	typedef std::unordered_map<AccountImpl::Id, jewel::Decimal> Map;

	void regenerate_map();

	void regenerate_instrument();

	/**
	 * Load the persistent aspects of AmalgamatedBudget from the
	 * database into memory; but do not \e regenerate the
	 * budget from the BudgetItems.
	 *
	 * Note this is "conceptually" const, even though it
	 * changes the internal state.
	 */
	void load() const;

	void load_balancing_account() const;

	/**
	 * Loads the instrument from the database, but does \e not \e regenerate
	 * the instrument from the saved BudgetItems.
	 */
	void load_instrument() const;

	/**
	 * Should only be called by load(), the first time AmalgamatedBudget is
	 * loaded.
	 */
	void load_map() const;

	/**
	 * This does the work of actually generating the map
	 * afresh from the saved BudgetItems. It's a bit dodgy
	 * that we're calling this const, as sometimes the map
	 * can completely change rather than just being loaded
	 * for the first time. But we do this so this can
	 * provide implementation details both for
	 * load_map() (const) and for refresh_map().
	 */
	void generate_map() const;

	/**
	 * Create and push Entries onto \e p_journal that reflect the
	 * AmalgamatedBudget. This does nothing to change the Repeaters,
	 * comment or other journal-level attributes of journal - it only
	 * affects the entries in p_journal. If there are already
	 * Entries in p_journal,
	 * these are all cleared prior to the new Entries being pushed on.
	 */
	void reflect_entries(DraftJournal& p_journal);

	/**
	 * Examines the Repeaters of p_journal. If there is exactly one
	 * Repeater, and it reflects the same Frequency as the AmalgamatedBudget
	 * Frequency, then that Repeater is left unchanged. Otherwise, all
	 * Repeaters are cleared from p_journal, and a new Repeater is
	 * pushed onto p_journal, with \e today as its next_date().
	 */
	void reflect_repeater(DraftJournal& p_journal);

	jewel::Decimal instrument_balancing_amount() const;

	bool mutable m_is_loaded;
	PhatbooksDatabaseConnection& m_database_connection;
	Frequency m_frequency;
	mutable std::unique_ptr<Map> m_map;

	// The DraftJournal that "effects" the AmalgamatedBudget
	mutable std::unique_ptr<DraftJournal> m_instrument; 

	// The the Account such that, when regenerating m_instrument, if the
	// journal is not otherwise balanced, any imbalance overflows
	// to this Account.
	mutable std::unique_ptr<Account> m_balancing_account;

};





}  // namespace phatbooks

#endif  // GUARD_amalgamated_budget_hpp_5997503626159161

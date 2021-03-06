/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_amalgamated_budget_hpp_5997503626159161
#define GUARD_amalgamated_budget_hpp_5997503626159161

#include "account.hpp"
#include "draft_journal.hpp"
#include "interval_type.hpp"
#include "frequency.hpp"
#include "dcm_exceptions.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace dcm
{

/**
 * An AmalgamatedBudget contains at most a single amount
 * per Account, and has a single frequency shared by all
 * the items in the AmalgamatedBudget.
 */
class AmalgamatedBudget
{
public:

    static void setup_tables(DcmDatabaseConnection& dbc);

    AmalgamatedBudget(DcmDatabaseConnection& p_database_connection);

    AmalgamatedBudget(AmalgamatedBudget const&) = delete;
    AmalgamatedBudget(AmalgamatedBudget&&) = delete;
    AmalgamatedBudget& operator=(AmalgamatedBudget const&) = delete;
    AmalgamatedBudget& operator=(AmalgamatedBudget&&) = delete;

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
    jewel::Decimal budget(sqloxx::Id p_account_id) const;

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
    (   std::vector<Frequency>& vec
    );

    static bool supports_frequency(Frequency const& p_frequency);

    /**
     * @returns a handle to the Account such that if the AmalgamatedBudget
     * is not otherwise "balanced", any imbalance is reconciled
     * to this Account.
     *
     * @throws UninitializedBalancingAccountException if
     * balancing account is uninitialized.
     */
    sqloxx::Handle<Account> balancing_account() const;

    /**
     * @returns the DraftJournal that serves as the "instrument"
     * by means of which the AmalgamatedBudget effects regular
     * distributions of budget amounts to budgeting envelopes
     * (Accounts).
     */
    sqloxx::Handle<DraftJournal> instrument() const;

private:

    typedef std::unordered_map<sqloxx::Id, jewel::Decimal> Map;

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
    void reflect_entries(sqloxx::Handle<DraftJournal> const& p_journal);

    /**
     * Examines the Repeaters of p_journal. If there is exactly one
     * Repeater, and it reflects the same Frequency as the AmalgamatedBudget
     * Frequency, then that Repeater is left unchanged. Otherwise, all
     * Repeaters are cleared from p_journal, and a new Repeater (or rather,
     * sqloxx::Handle<Repeater>) is
     * pushed onto p_journal, with \e today as its next_date().
     */
    void reflect_repeater(sqloxx::Handle<DraftJournal> const& p_journal);

    jewel::Decimal instrument_balancing_amount() const;

    bool mutable m_is_loaded;
    DcmDatabaseConnection& m_database_connection;
    Frequency m_frequency;
    std::unique_ptr<Map> mutable m_map;

    // The DraftJournal that "effects" the AmalgamatedBudget
    sqloxx::Handle<DraftJournal> mutable m_instrument; 

    // Represents the Account such that, when regenerating m_instrument, if
    // the journal is not otherwise balanced, any imbalance overflows
    // to this Account.
    sqloxx::Handle<Account> mutable m_balancing_account;

};

}  // namespace dcm

#endif  // GUARD_amalgamated_budget_hpp_5997503626159161

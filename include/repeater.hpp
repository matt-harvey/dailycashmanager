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

#ifndef GUARD_repeater_impl_hpp_7204316857831701
#define GUARD_repeater_impl_hpp_7204316857831701

#include "date.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "dcm_database_connection.hpp"
#include "proto_journal.hpp"
#include "repeater_firing_result.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/persistent_object.hpp>
#include <memory>
#include <string>
#include <vector>

namespace dcm
{

// begin forward declarations

class DraftJournal;
class Frequency;

// end forward declarations

/**
 * Instances of this class serve as "alarms" that "fire" at regular intervals.
 * On firing, a \c Repeater triggers an automatic journal posting, and
 * updates itself to await the next firing.
 *
 * Important properties of a Repeater are: (a) the Journal that it causes
 * the posting of, when the repeater fires; and (b) the time between each
 * firing. The time between firings is represented by a number of units, and
 * a type of unit. So, a journal that fires every 3 weeks has \e weeks as its
 * interval type (\e represented by the IntervalType enum), and 3 as the
 * number of units (\e interval_units). At any point in time, a Repeater
 * also has (c) its \e next_date, the date at which it will next fire.
 *
 * Repeaters are associated with DraftJournals. When a Repeater fires, it
 * causes an OrdinaryJournal to be "cloned from" the DraftJournal, and then
 * posted.
 *
 * Client code should not deal with Repeater instances directly, but
 * only ever via sqloxx::Handle<Repeater>.
 */
class Repeater:
	public sqloxx::PersistentObject<Repeater, DcmDatabaseConnection>
{
public:
	
	typedef sqloxx::PersistentObject
	<	Repeater,
		DcmDatabaseConnection
	>	PersistentObject;

	/**
	 * Sets up tables in the database required for the persistence
	 * of Repeater objects.
	 */
	typedef sqloxx::IdentityMap<Repeater> IdentityMap;

	static void setup_tables(DcmDatabaseConnection& dbc);

	/**
	 * Construct a fresh Repeater with no Id, not yet persisted to the
	 * database.
	 *
	 * Cannot be called except by IdentityMap. This is enforced by by
	 * Signature parameter.
	 */
	Repeater
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Get a Repeater by Id from the database.
	 *
	 * Cannot be called except by IdentityMap. This is enforced by the
	 * Signature parameter.
	 */
	Repeater
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// copy constructor is private

	Repeater(Repeater&&) = delete;
	Repeater& operator=(Repeater const&) = delete;
	Repeater& operator=(Repeater&&) = delete;

	~Repeater();

	/**
	 * @throws InvalidFrequencyException in the event that the "next date"
	 * has already been set for this Repeater and \e p_frequency is
	 * incompatible with that "next date".
	 *
	 * @see \e is_valid_date_for_interval_type
	 */
	void set_frequency(Frequency const& p_frequency);

	/**
	 * @throws InvalidRepeaterDateException in the event that the Frequency
	 * has already been set for this Repeater and \e p_next_date
	 * incompatible with that frequency.
	 *
	 * @throws InvalidRepeaterDateException in the event that \e p_next_date
	 * is earlier than the database_connection().entity_creation_date().
	 *
	 * @see \e is_valid_date_for_interval_type
	 */
	void set_next_date(boost::gregorian::date const& p_next_date);

	/**
	 * Associate the Repeater with a particular DraftJournal, by
	 * passing the id of the DraftJournal to \e p_journal_id.
	 * This function should \e not normally be called. The usual way
	 * to associate a Repeater with a DraftJournal is via
	 * \e DraftJournal::push_repeater(...). The DraftJournal class
	 * takes care of assigning the correct journal id to Repeaters,
	 * without client code needing to do this directly.
	 */
	void set_journal_id(sqloxx::Id p_journal_id);
		
	Frequency frequency();

	/**
	 * Calling next_date() (which is equivalent to calling next_date(0)), will
	 * return the date when the Repeater is next due to fire. Calling
	 * next_date(1) will return the date when the Repeater is next due to
	 * fire after \e that. Etc.
	 *
	 * @throws UnsafeArithmeticException in the extremely unlikely event of
	 * arithmetic overflow during execution.
	 */
	boost::gregorian::date next_date
	(	std::vector<boost::gregorian::date>::size_type n = 0
	);

	/**
	 * Post an OrdinaryJournal - based on this Repeater's DraftJournal -
	 * with the date of the OrdinaryJournal being next_date(0). Then
	 * update \e next_date internally to (what was) next_date(1).
	 *
	 * If the DraftJournal is database_connection().budget_instrument(),
	 * and is devoid of Entries, then an OrdinaryJournal is not
	 * actually posted; however the next_date is still updated. In this
	 * case, an OrdinaryJournal will still be returned, but it will have id,
	 * no Entries and no other attributes.
	 * This behaviour is to avoid mystifying the user with
	 * empty journal posting notifications in case they have not
	 * yet set up any BudgetItems.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	sqloxx::Handle<OrdinaryJournal> fire_next();
	
	sqloxx::Handle<DraftJournal> draft_journal();

	void swap(Repeater& rhs);

	static std::string exclusive_table_name();
	static std::string primary_key_name();

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * 	\e id,\n
	 * 	\e database_connection, or\n
	 * 	\e journal_id.
	 */
	void mimic(Repeater& rhs);

private:

	Repeater(Repeater const& rhs);
	void do_load() override;
	void do_save_existing() override;
	void do_save_new() override;
	void do_ghostify() override;
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct RepeaterData;

	std::unique_ptr<RepeaterData> m_data;
};

// Free functions

/**
 * Attempts to bring Repeaters up to date (thereby posting auto posted
 * journals), returning a sequence of RepeaterFiringResults, each of which
 * conveys the result of a single firing attempt. The sequence is sorted by
 * date of firing (or attempted firing).
 *
 * @param p_database_connection the database connection with which the
 * Repeaters we want to update are associated.
 *
 * @param p_target_date the date to which we want the Repeaters brought up to
 * date.
 */
std::vector<RepeaterFiringResult> update_repeaters
(	DcmDatabaseConnection& p_database_connection,
	boost::gregorian::date p_target_date = today()
);

}  // namespace dcm

#endif  // GUARD_repeater_impl_hpp_7204316857831701

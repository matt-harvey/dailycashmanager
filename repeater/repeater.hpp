// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_repeater_hpp_1481954204608665
#define GUARD_repeater_hpp_1481954204608665

#include "date.hpp"
#include "draft_journal.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "repeater_impl.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <memory>
#include <string>
#include <vector>

namespace phatbooks
{


class DraftJournal;


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
 * @todo Prevent next_date from being set to anything other than the last
 * day of the month in the event that the interval type is month_ends; and
 * prevent next_date from being set later than the 27th in the event the
 * interval_type is months (as opposed to month_ends).
 *
 * @todo Should it be possible to have Repeaters that have reminders attached
 * them, as well as just transactions?
 */
class Repeater:
	public PhatbooksPersistentObject<RepeaterImpl>
{
public:
	typedef
		PhatbooksPersistentObject<RepeaterImpl>
		PhatbooksPersistentObject;
	typedef PhatbooksPersistentObjectBase::Id Id;

	typedef interval_type::IntervalType IntervalType;

	/**
	 * Sets up tables in the database required for the persistence
	 * of Repeater objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	/**
	 * Construct a "raw" Repeater, that will not yet correspond to any
	 * particular object in the database.
	 */
	explicit Repeater
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Get a Repeater by id from the database. Throws if there is not
	 * Repeater with this id.
	 */
	Repeater
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * @returns a Repeater (purportedly) identified by id in the
	 * database. This function is a relatively fast way to get a
	 * Repeater instance with an id; however it does not check whether
	 * a Repeater with this id actually exists - this is the caller's
	 * responsibility.
	 */
	static Repeater create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);


	void set_frequency(Frequency const p_frequency);

	/**
	 * Associated the Repeater with a particular DraftJournal, by
	 * passing the id of the DraftJournal to \e p_journal_id.
	 * This function should \e not normally be used. The usual way
	 * to associate a Repeater with a DraftJournal is to pass a
	 * \e shared_ptr to the Repeater to
	 * \e DraftJournal::push_repeater(...). The DraftJournal class
	 * takes care of assigning the correct journal id to Repeaters,
	 * without client code needing to do this directly.
	 */
	void set_journal_id(DraftJournal::Id p_journal_id);

	/**
	 * Set the date when the Repeater will next fire.
	 *
	 * @throws InvalidRepeaterDateException if the date is earlier
	 * than database_connection().entity_creation_date().
	 */
	void set_next_date(boost::gregorian::date const& p_next_date);

	/**
	 * Find the firings that are due to occur for this Repeater
	 * up till and including \e limit.
	 */
	std::shared_ptr<std::vector<boost::gregorian::date> >
	firings_till(boost::gregorian::date const& limit);

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
	 * @returns the just-posted OrdinaryJournal.
	 */
	OrdinaryJournal fire_next();

	Frequency frequency() const;

	/**
	 * Calling next_date() (which is equivalent to calling next_date(0)), will
	 * return the date when the Repeater is next due to fire. Calling
	 * next_date(1) will return the date when the Repeater is next due to
	 * fire after \e that. Etc.
	 */
	boost::gregorian::date next_date
	(	std::vector<boost::gregorian::date>::size_type n = 0
	) const;

	DraftJournal draft_journal() const;

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * 	\e id,\n
	 * 	\e database_connection, or\n
	 * 	\e journal_id.
	 */
	void mimic(Repeater const& rhs);

private:
	Repeater(sqloxx::Handle<RepeaterImpl> const& p_handle);
};


// Free functions

/**
 * Bring Repeaters up to date (thereby posting auto posted journals),
 * returning a shared_ptr to a list containing the resulting
 * OrdinaryJournals, sorted by the order in which they have been
 * posted, from earliest to latest.
 */
std::shared_ptr<std::list<OrdinaryJournal> >
update_repeaters
(	PhatbooksDatabaseConnection& dbc,
	boost::gregorian::date d = today()
);


}  // namespace phatbooks

#endif  // GUARD_repeater_hpp_1481954204608665

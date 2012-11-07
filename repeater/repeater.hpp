#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

/** \file repeater.hpp
 *
 * \brief Header file pertaining to Repeater class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "date.hpp"
#include "journal.hpp"
#include "general_typedefs.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


namespace phatbooks
{

/**
 * Instances of this class serve as "alarms" that "fire" at regular intervals.
 * On firing, a \c Repeater triggers an automatic journal posting, and updates
 * itself to await the next firing.
 *
 * Important properties of a Repeater are: (a) the Journal that it causes
 * the posting of, when the repeater fires; and (b) the time between each
 * firing. The time between firings is represented by a number of units, and
 * a type of unit. So, a journal that fires every 3 weeks has \e weeks as its
 * interval type (\e represented by the IntervalType enum), and 3 as the
 * number of units (\e interval_units). At any point in time, a Repeater also
 * has (c) its \e next_date, the date at which it will next fire.
 *
 * @todo Write firing method.
 *
 * @todo Write a method somewhere in either the session-level code, or in
 * PhatbooksDatabaseConnection, that loads Repeater objects from the database,
 * and inspects them all and fires those that are due.
 */
class Repeater:
	public sqloxx::PersistentObject,
	private boost::noncopyable
{
public:

	typedef sqloxx::PersistentObject PersistentObject;
	typedef PersistentObject::Id Id;

	/**
	 * Sets up tables in the database required for the persistence
	 * of Repeater objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	/**
	 * Initialize a "raw" Repeater, that will not yet correspond to any
	 * particular object in the database.
	 */
	explicit
	Repeater
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
	);

	/**
	 * Get a Repeater by id from the database.
	 */
	Repeater
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		Id p_id
	);

	/**
	 * Enumerated type representing different
	 * date interval.
	 *
	 * Should be self-explanatory; except note that
	 * \c month_ends represents an interval between the
	 * last day of one month and the last day of the next,
	 * whereas \c months simply represents an interval of
	 * one month with no particular reference to the end
	 * of the month.
	 *
	 * Note the numbering of the enumeration is significant.
	 * The class PhatbooksDatabaseConnection relies on the
	 * numbering shown here.
	 */
	enum IntervalType
	{
		days = 1,
		weeks,
		months,
		month_ends
	};


	void set_interval_type(IntervalType p_interval_type);

	void set_interval_units(int p_interval_units);

	void set_next_date(boost::gregorian::date const& p_next_date);

	/**
	 * Associate the Repeater with a particular DraftJournal, by passing
	 * the id of the DraftJournal. (As a PersistentObject, a DraftJournal can
	 * be identified from its id.) Note this function should normally
	 * NOT be used. The usual way to associate a Repeater with a
	 * DraftJournal is to pass a shared_ptr to the Repeater to
	 * DraftJournal::add_repeater.
	 */
	void set_journal_id(Journal::Id p_journal_id);
		
	IntervalType interval_type();

	int interval_units();

	boost::gregorian::date next_date();

	Journal::Id journal_id();

private:

	// Inherited virtual member functions

	virtual void do_load_all();

	/**
	 * @todo Implement this properly.
	 * WARNING This is not properly implemented.
	 */
	virtual void do_save_existing_all()
	{
	}

	/**
	 * @todo Implement this properly.
	 * WARNING This is not properly implemented.
	 */
	virtual void do_save_existing_partial()
	{
	}

	virtual void do_save_new_all();

	virtual std::string do_get_table_name() const;

	// Data members
	
	boost::optional<IntervalType> m_interval_type;
	boost::optional<int> m_interval_units;
	boost::optional<DateRep> m_next_date;
	boost::optional<Journal::Id> m_journal_id;
};






}  // namespace phatbooks

#endif  // GUARD_repeater_hpp

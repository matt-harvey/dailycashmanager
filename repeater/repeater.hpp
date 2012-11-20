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
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


namespace sqloxx
{
	class SharedSQLStatement;  // forward declaration
}  // namespace sqloxx


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
	public sqloxx::PersistentObject<Repeater, PhatbooksDatabaseConnection>
{
public:

	typedef sqloxx::PersistentObject<Repeater, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;

	/**
	 * Sets up tables in the database required for the persistence
	 * of Repeater objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" Repeater, that will not yet correspond to any
	 * particular object in the database.
	 */
	explicit
	Repeater
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection
	);

	/**
	 * Get a Repeater by id from the database.
	 */
	Repeater
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection,
		Id p_id
	);

	~Repeater();

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

	void swap(Repeater& rhs);

	static std::string primary_table_name();
private:

	Repeater(Repeater const& rhs);
	void do_load();
	void do_save_existing();
	void do_save_new();
	void process_saving_statement(sqloxx::SharedSQLStatement& statement);

	struct RepeaterData
	{
		boost::optional<IntervalType> interval_type;
		boost::optional<int> interval_units;
		boost::optional<DateRep> next_date;
		boost::optional<Journal::Id> journal_id;
	};

	boost::scoped_ptr<RepeaterData> m_data;
};







}  // namespace phatbooks

#endif  // GUARD_repeater_hpp

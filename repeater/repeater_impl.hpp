#ifndef GUARD_repeater_impl_hpp
#define GUARD_repeater_impl_hpp

/** \file repeater_impl.hpp
 *
 * \brief Header file pertaining to RepeaterImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "date.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include <sqloxx/persistent_object.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace phatbooks
{




/**
 * Instances of this class serve as "alarms" that "fire" at regular intervals.
 * On firing, a \c RepeaterImpl triggers an automatic journal posting, and updates
 * itself to await the next firing.
 *
 * Important properties of a RepeaterImpl are: (a) the Journal that it causes
 * the posting of, when the repeater fires; and (b) the time between each
 * firing. The time between firings is represented by a number of units, and
 * a type of unit. So, a journal that fires every 3 weeks has \e weeks as its
 * interval type (\e represented by the IntervalType enum), and 3 as the
 * number of units (\e interval_units). At any point in time, a RepeaterImpl also
 * has (c) its \e next_date, the date at which it will next fire.
 *
 * @todo Write firing method.
 *
 * @todo Prevent next_date from being set to anything other than the last
 * day of the month in the event that the interval type is month_ends; and
 * prevent next_date from being set later than the 27th in the event the
 * interval_type is months (as opposed to month_ends).
 *
 * @todo Write a method somewhere in either the session-level code, or in
 * PhatbooksDatabaseConnection, that loads RepeaterImpl objects from the database,
 * and inspects them all and fires those that are due.
 */
class RepeaterImpl:
	public sqloxx::PersistentObject<RepeaterImpl, PhatbooksDatabaseConnection>
{
public:
	
	typedef sqloxx::PersistentObject<RepeaterImpl, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;
	typedef sqloxx::IdentityMap<RepeaterImpl, PhatbooksDatabaseConnection>
		IdentityMap;
	typedef interval_type::IntervalType IntervalType;

	/**
	 * Sets up tables in the database required for the persistence
	 * of RepeaterImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" RepeaterImpl, that will not yet correspond to any
	 * particular object in the database.
	 */
	explicit
	RepeaterImpl
	(	IdentityMap& p_identity_map
	);

	/**
	 * Get a RepeaterImpl by id from the database.
	 */
	RepeaterImpl
	(	IdentityMap& p_identity_map,	
		Id p_id
	);

	~RepeaterImpl();

	void set_interval_type(IntervalType p_interval_type);

	void set_interval_units(int p_interval_units);

	void set_next_date(boost::gregorian::date const& p_next_date);

	/**
	 * Associate the RepeaterImpl with a particular DraftJournal, by passing
	 * the id of the DraftJournal. (As a PersistentObject, a DraftJournal can
	 * be identified from its id.) Note this function should normally
	 * NOT be used. The usual way to associate a RepeaterImpl with a
	 * DraftJournal is to pass a shared_ptr to the RepeaterImpl to
	 * DraftJournal::add_repeater.
	 */
	void set_journal_id(ProtoJournal::Id p_journal_id);
		
	IntervalType interval_type();

	int interval_units();

	/**
	 * @param n the number of leaps into the future beyond the the next date,
	 * leaping by interval_units() intervals each time, where each interval
	 * is given by interval_type(). If n is 0 then it is simply the next
	 * firing date that is returned.
	 *
	 * @throws UnsafeArithmeticException in the extremely unlikely event of
	 * arithmetic overflow during execution.
	 */
	boost::gregorian::date next_date
	(	std::vector<boost::gregorian::date>::size_type n
	);

	/**
	 * Find the list of firings that are due to occur for this RepeaterImpl
	 * up till and including \e limit.
	 *
	 * @throws UnsafeArithmeticException in the extremely unlikely event of
	 * arithmetic overflow during calculation.
	 *
	 * @returns a smart pointer to a vector into which the
	 * list of firings occur in chronological order from
	 * soonest to latest.
	 */
	boost::shared_ptr<std::vector<boost::gregorian::date> >
	firings_till(boost::gregorian::date const& limit);

	/**
	 * Post an OrdinaryJournal - based on this Repeater's DraftJournal -
	 * with the date of the OrdinaryJournal being next_date(0). Then
	 * update next_date internally to (what was) next_date(1).
	 *
	 * @returns The just-posted OrdinaryJournal.
	 *
	 * @todo Determine exceptions and exception-safety. These are important
	 * here.
	 *
	 * @todo Testing
	 */
	OrdinaryJournal fire_next();
	

	ProtoJournal::Id journal_id();

	void swap(RepeaterImpl& rhs);

	static std::string primary_table_name();
	static std::string primary_key_name();
private:

	RepeaterImpl(RepeaterImpl const& rhs);
	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct RepeaterData;

	boost::scoped_ptr<RepeaterData> m_data;
};


struct RepeaterImpl::RepeaterData
{
	boost::optional<RepeaterImpl::IntervalType> interval_type;
	boost::optional<int> interval_units;
	boost::optional<DateRep> next_date;
	boost::optional<ProtoJournal::Id> journal_id;
};





}  // namespace phatbooks

#endif  // GUARD_repeater_impl_hpp

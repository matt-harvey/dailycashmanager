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
#include <boost/date_time/gregorian/gregorian.hpp>


namespace phatbooks
{

/**
 * Instances of this class serve as "alarms" that "fire" at regular intervals.
 * On firing, a \c Repeater triggers an automatic journal posting, and updates
 * itself to await the next firing.
 *
 * @todo Write firing method.
 *
 * @todo The Repeater currently has not mechanism for associating itself
 * with a journal. It should be associated using a journal_id, rather than
 * having a link with the object. The journal_id in question will only ever
 * refer to a journal stored in the draft_journals table.
 *
 * @todo Write a method somewhere in either the session-level code, or in
 * PhatbooksDatabaseConnection, that loads Repeater objects from the database,
 * and inspects them all and fires those that are due.
 */
class Repeater
{
public:

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

	/** 
	 * @param p_journal The journal the posting of which is triggered
	 * by the firing of this \c Repeater.
	 *
	 * @param p_interval_type The \c IntervalType such that the \c Repeater
	 * "fires" after every \c p_interval_units intervals of
	 * \c p_interval_type.
	 *
	 * @param p_interval_units The number of intervals of \c p_interval_type
	 * such that duration between firings of the \c Repeater is this many
	 * intervals of that type.
	 *
	 * @param p_next_date The date on which the \c Repeater is due to fire
	 * next.
	 *
	 * This constructor does not throw.
	 */
	Repeater
	(	IntervalType p_interval_type,
		int p_interval_units,
		boost::gregorian::date const& p_next_date
	);

private:
	IntervalType m_interval_type;
	int m_interval_units;
	DateRep m_next_date;
};




}  // namespace phatbooks

#endif  // GUARD_repeater_hpp

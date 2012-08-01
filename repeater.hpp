#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

#include "date.hpp"
#include "journal.hpp"
#include <boost/shared_ptr.hpp>

namespace phatbooks
{

/** Repeater
 * 
 * @todo document constructor properly.
 *
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

	/** Constructor
	 */
	Repeater
	(	boost::shared_ptr<Journal> p_journal,
		IntervalType p_interval_type,
		int p_interval_units,
		DateType p_next_date
	);

private:
	boost::shared_ptr<Journal> m_journal;
	IntervalType m_interval_type;
	int m_interval_units;
	DateType m_next_date;
};




}  // namespace phatbooks

#endif  // GUARD_repeater_hpp

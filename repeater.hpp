#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

#include "general_typedefs.hpp"
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
	// enum order is significant, as the database contains
	// a table with primary keys in this order
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

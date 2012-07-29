#include "repeater.hpp"
#include "journal.hpp"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

namespace phatbooks
{

Repeater::Repeater
(	shared_ptr<Journal> p_journal,
	IntervalType p_interval_type,
	int p_interval_units,
	DateType p_next_date
):
	m_journal(p_journal),
	m_interval_type(p_interval_type),
	m_interval_units(p_interval_units),
	m_next_date(p_next_date)
{
}




}  // namespace phatbooks

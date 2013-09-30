#ifndef GUARD_repeater_data_hpp_9738352792215698
#define GUARD_repeater_data_hpp_9738352792215698

#include "date.hpp"
#include "frequency.hpp"
#include "repeater.hpp"
#include <boost/optional.hpp>
#include <sqloxx/general_typedefs.hpp>

namespace phatbooks
{

struct Repeater::RepeaterData
{
	boost::optional<Frequency> frequency;
	boost::optional<DateRep> next_date;
	boost::optional<sqloxx::Id> journal_id;
};

}  // namespace phatbooks

#endif  // GUARD_repeater_data_hpp_9738352792215698

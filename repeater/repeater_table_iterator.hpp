#ifndef GUARD_repeater_table_iterator_hpp_8880911641233094
#define GUARD_repeater_table_iterator_hpp_8880911641233094

#include "repeater.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<Repeater, PhatbooksDatabaseConnection>
	RepeaterTableIterator;

}  // namespace phatbooks

#endif  // GUARD_repeater_table_iterator_hpp_8880911641233094

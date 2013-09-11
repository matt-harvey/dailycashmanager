#ifndef GUARD_repeater_table_iterator_hpp
#define GUARD_repeater_table_iterator_hpp

#include "repeater.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<Repeater, PhatbooksDatabaseConnection>
	RepeaterTableIterator;

}  // namespace phatbooks

#endif  // GUARD_repeater_table_iterator_hpp

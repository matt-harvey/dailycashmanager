#ifndef GUARD_account_table_iterator_hpp
#define GUARD_account_table_iterator_hpp

#include "account.hpp"
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{


typedef
	sqloxx::TableIterator<Account, PhatbooksDatabaseConnection>
	AccountTableIterator;


}  // namespace phatbooks

#endif  // GUARD_account_table_iterator_hpp

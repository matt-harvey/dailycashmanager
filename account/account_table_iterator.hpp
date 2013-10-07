// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_table_iterator_hpp_048275898233184546
#define GUARD_account_table_iterator_hpp_048275898233184546

#include "account.hpp"
#include <sqloxx/handle.hpp>
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<sqloxx::Handle<Account> >
	AccountTableIterator;

/**
 * @returns an AccountTableIterator that iterates over all Accounts
 * ordered by name.
 */
AccountTableIterator
make_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

/**
 * @returns an AccountTableIterator that iterates over all Accounts,
 * ordered first by AccountType, then by name.
 */
AccountTableIterator
make_type_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

}  // namespace phatbooks

#endif  // GUARD_account_table_iterator_hpp_048275898233184546

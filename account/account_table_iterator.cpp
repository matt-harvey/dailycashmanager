// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account_table_iterator.hpp"

namespace phatbooks
{

AccountTableIterator
make_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return AccountTableIterator
	(	p_database_connection,
		"select account_id from accounts order by name"
	);
}

AccountTableIterator
make_type_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return AccountTableIterator
	(	p_database_connection,
		"select account_id from accounts order by account_type_id, name"
	);
}





}  // namespace phatbooks

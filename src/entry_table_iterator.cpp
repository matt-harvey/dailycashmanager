// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "entry_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include <jewel/assert.hpp>
#include <sstream>

using std::ostringstream;

namespace phatbooks
{

EntryTableIterator
make_date_ordered_actual_ordinary_entry_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	ostringstream oss;
	oss << "select entry_id from entries inner join ordinary_journal_detail "
		<< "using(journal_id) join journals using(journal_id) "
		<< "where transaction_type_id != "
		<< static_cast<int>(non_actual_transaction_type())
		<< " order by date";
	return EntryTableIterator
	(	p_database_connection,
		oss.str()
	);
}


}  // namespace phatbooks

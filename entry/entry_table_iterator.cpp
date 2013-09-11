#include "entry_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include <jewel/assert.hpp>

namespace phatbooks
{

EntryTableIterator
make_date_ordered_actual_ordinary_entry_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
#	ifndef NDEBUG
		// Ensure we are picking all and only the
		// actual transactions.
		int const target_non_actual_type = 3;
		int i = 0;
		int const lim =
			static_cast<int>(transaction_type::num_transaction_types);
		for ( ; i != lim; ++i)
		{
			transaction_type::TransactionType const ttype =
				static_cast<transaction_type::TransactionType>(i);
			if (ttype == target_non_actual_type)
			{
				JEWEL_ASSERT (!transaction_type_is_actual(ttype));
			}
			else
			{
				JEWEL_ASSERT (transaction_type_is_actual(ttype));
			}
		}
#	endif
	return EntryTableIterator
	(	p_database_connection,
		"select entry_id from entries inner join ordinary_journal_detail "
		"using(journal_id) join journals using(journal_id) "
		"where transaction_type_id != 3 order by date"
	);
}


}  // namespace phatbooks

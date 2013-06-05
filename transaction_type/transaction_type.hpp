#ifndef GUARD_transaction_type_hpp
#define GUARD_transaction_type_hpp

#include "b_string.hpp"
#include <vector>

namespace phatbooks
{

namespace transaction_type
{
	/**
	 * Different types of "transactions" (i.e., Journals), as they appear
	 * to the \e user. This affects the way we present Journal-related
	 * information to the user, for example whether we use the words
	 * "amount spent", "amount earned" or "amount transferred" when
	 * requesting the transaction amount from the user.
	 */
	enum TransactionType
	{
		expenditure_transaction = 0,
		revenue_transaction,
		balance_sheet_transaction,
		envelope_transaction,
		generic_transaction,
		num_transaction_types  // do not insert enumerators below here
	};

}  // namespace transaction_type


std::vector<transaction_type::TransactionType> const&
transaction_types();

BString
transaction_type_to_verb
(	transaction_type::TransactionType p_transaction_type
);

bool
transaction_type_is_actual
(	transaction_type::TransactionType p_transaction_type
);


}  // namespace phatbooks

#endif  // GUARD_transaction_type_hpp

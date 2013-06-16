#ifndef GUARD_transaction_type_hpp
#define GUARD_transaction_type_hpp

#include "account_type.hpp"
#include "b_string.hpp"
#include <vector>

namespace phatbooks
{

class Account;  // forward declaration

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


/**
 * @returns a vector of all the TransactionTypes.
 */
std::vector<transaction_type::TransactionType> const&
transaction_types();

/**
 * @returns a natural language verb corresponding to a p_tranaction_type;
 * for example, "Spend".
 */
BString
transaction_type_to_verb
(	transaction_type::TransactionType p_transaction_type
);

/**
 * @returns true if and only if p_transaction_type is a type of actual
 * transaction, as opposed to budget transaction.
 */
bool
transaction_type_is_actual
(	transaction_type::TransactionType p_transaction_type
);

/**
 * @returns a vector of AccountTypes appropriate for a given
 * TransactionType's \e source Account.
 */
std::vector<account_type::AccountType> const&
source_account_types
(	transaction_type::TransactionType p_transaction_type
);

/**
 * @returns a vector of AccountTypes appropriate for a given
 * TransactionType's \e destination Account.
 */
std::vector<account_type::AccountType> const&
destination_account_types
(	transaction_type::TransactionType p_transaction_type
);

/**
 * @returns the TransactionType that it would be natural to assume
 * to be applicable, for a given pair of Accounts. For example,
 * if one is an expense Account and the other is an asset Account,
 * then it would be natural to assume we have an expenditure_transaction.
 */
transaction_type::TransactionType
natural_transaction_type(Account const& account_x, Account const& account_y);


}  // namespace phatbooks

#endif  // GUARD_transaction_type_hpp

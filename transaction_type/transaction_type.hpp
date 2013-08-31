// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_type_hpp
#define GUARD_transaction_type_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <wx/string.h>
#include <cassert>
#include <set>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class PhatbooksDatabaseConnection;

// End forward declarations

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
 * @returns a vector containing every TransactionType for which
 * there exist, saved in p_database_connection, Accounts of the
 * requisite AccountTypes for that TransactionType.
 */
std::vector<transaction_type::TransactionType>
available_transaction_types
(	PhatbooksDatabaseConnection& p_database_connection
);

/**
 * @returns a natural language verb or or other phrase corresponding to
 * a p_tranaction_type; for example, "Spend".
 */
wxString
transaction_type_to_verb
(	transaction_type::TransactionType p_transaction_type
);

/**
 * @returns a TransactionType from a natural language language verb or
 * other phrase (e.g. "Spend"). The correspondence is case-sensitive and
 * is the same as that used in the transaction_type_to_verb(...) function.
 *
 * @throws InvalidTransactionTypeException if p_phrase does not correspond to
 * any TransactionType.
 */
transaction_type::TransactionType
transaction_type_from_verb(wxString const& p_phrase);

/**
 * @returns true if and only if p_transaction_type is a type of actual
 * transaction, as opposed to budget transaction.
 */
bool transaction_type_is_actual
(	transaction_type::TransactionType p_transaction_type
);

/**
 * Check for validity of p_transaction_type, using assert statement.
 * Terminates program if invalid while NDEBUG is not defined; otherwise
 * does nothing.
 */
void assert_transaction_type_validity
(	transaction_type::TransactionType p_transaction_type
);

}  // namespace phatbooks


#include "account_type.hpp"

namespace phatbooks
{

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

void
source_super_types
(	transaction_type::TransactionType p_transaction_type,
	std::set<account_super_type::AccountSuperType>& out
);

void
destination_super_types
(	transaction_type::TransactionType p_transaction_type,
	std::set<account_super_type::AccountSuperType>& out
);


/**
 * @returns the TransactionType that it would be natural to assume
 * to be applicable, for a given pair of Accounts. For example,
 * if one is an expense Account and the other is an asset Account,
 * then it would be natural to assume we have an expenditure_transaction.
 */
transaction_type::TransactionType
natural_transaction_type(Account const& account_x, Account const& account_y);


// Inline implementations

inline
void
assert_transaction_type_validity
(	transaction_type::TransactionType p_transaction_type
)
{
#	ifndef NDEBUG
		int const ttype_as_int = static_cast<int>(p_transaction_type);
		int const num_ttypes_as_int =
			static_cast<int>(transaction_type::num_transaction_types);
		assert (ttype_as_int >= 0);
		assert (ttype_as_int < num_ttypes_as_int);
#	endif  // NDEBUG
}


}  // namespace phatbooks

#endif  // GUARD_transaction_type_hpp

/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_transaction_type_hpp_36120869003292577
#define GUARD_transaction_type_hpp_36120869003292577

#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <wx/string.h>
#include <set>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class PhatbooksDatabaseConnection;

// End forward declarations

/**
 * Different types of "transactions" (i.e., Journals), as they appear
 * to the \e user. This affects the way we present Journal-related
 * information to the user, for example whether we use the words
 * "amount spent", "amount earned" or "amount transferred" when
 * requesting the transaction amount from the user.
 */
enum class TransactionType: unsigned char
{
	expenditure = 0,
	revenue,
	balance_sheet,
	envelope,
	generic,
	num_transaction_types  // do not insert enumerators below here
};


/**
 * @returns a vector of all the TransactionTypes.
 */
std::vector<TransactionType> const&
transaction_types();

/**
 * @returns a vector containing every TransactionType for which
 * there exist, saved in p_database_connection, Accounts of the
 * requisite AccountTypes for that TransactionType.
 */
std::vector<TransactionType>
available_transaction_types
(	PhatbooksDatabaseConnection& p_database_connection
);

/**
 * @returns a natural language verb or or other phrase corresponding to
 * a p_tranaction_type; for example, "Spend".
 */
wxString
transaction_type_to_verb
(	TransactionType p_transaction_type
);

/**
 * @returns a TransactionType from a natural language language verb or
 * other phrase (e.g. "Spend"). The correspondence is case-sensitive and
 * is the same as that used in the transaction_type_to_verb(...) function.
 *
 * @throws InvalidTransactionTypeException if p_phrase does not correspond to
 * any TransactionType.
 */
TransactionType
transaction_type_from_verb(wxString const& p_phrase);

/**
 * @returns true if and only if p_transaction_type is a type of actual
 * transaction, as opposed to budget transaction.
 */
bool transaction_type_is_actual
(	TransactionType p_transaction_type
);

/**
 * Check for validity of p_transaction_type, using assert statement.
 * Terminates program if invalid while NDEBUG is not defined; otherwise
 * does nothing.
 */
void assert_transaction_type_validity
(	TransactionType p_transaction_type
);

/**
 * @returns a TransactionType which is non-actual, which is guaranteed to
 * be the sole non-actual TransactionType;
 */
TransactionType non_actual_transaction_type();


}  // namespace phatbooks


#include "account_type.hpp"

namespace phatbooks
{

/**
 * @returns a vector of AccountTypes appropriate for a given
 * TransactionType's \e source Account.
 */
std::vector<AccountType> const&
source_account_types
(	TransactionType p_transaction_type
);

/**
 * @returns a vector of AccountTypes appropriate for a given
 * TransactionType's \e destination Account.
 */
std::vector<AccountType> const&
destination_account_types
(	TransactionType p_transaction_type
);

void
source_super_types
(	TransactionType p_transaction_type,
	std::set<AccountSuperType>& out
);

void
destination_super_types
(	TransactionType p_transaction_type,
	std::set<AccountSuperType>& out
);


/**
 * @returns the TransactionType that it would be natural to assume
 * to be applicable, for a given pair of Accounts. For example,
 * if one is an expense Account and the other is an asset Account,
 * then it would be natural to assume we have an expenditure_transaction.
 */
TransactionType natural_transaction_type
(	sqloxx::Handle<Account> const& account_x,
	sqloxx::Handle<Account> const& account_y
);


// Inline implementations

inline
void
assert_transaction_type_validity
(	TransactionType p_transaction_type
)
{
#	ifndef NDEBUG
		int const ttype_as_int = static_cast<int>(p_transaction_type);
		int const num_ttypes_as_int =
			static_cast<int>(TransactionType::num_transaction_types);
		JEWEL_ASSERT (ttype_as_int >= 0);
		JEWEL_ASSERT (ttype_as_int < num_ttypes_as_int);
#	endif  // NDEBUG
	return;
}



}  // namespace phatbooks

#endif  // GUARD_transaction_type_hpp_36120869003292577

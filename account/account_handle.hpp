// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_handle_hpp_7530966385980586
#define GUARD_account_handle_hpp_7530966385980586

#include "account.hpp"
#include <sqloxx/handle.hpp>

namespace phatbooks
{

class Commodity;
class PhatbooksDatabaseConnection;

typedef sqloxx::Handle<Account> AccountHandle;


/**
 * Represents an Account (via an AccountHandle), together with an amount
 * representing its opening balance. This is simply a convenient way of
 * grouping these two pieces of information together; hence a
 * struct.
 */
struct AugmentedAccount
{
	AugmentedAccount
	(	PhatbooksDatabaseConnection& p_database_connection,
		Commodity const& p_commodity
	);
	AugmentedAccount
	(	AccountHandle const& p_account,
		jewel::Decimal const& p_technical_opening_balance
	);
	AccountHandle account;
	jewel::Decimal technical_opening_balance;
};


/* Free functions ********************************************/

std::vector<AccountType> balance_sheet_account_types();
std::vector<AccountType> pl_account_types();


}  // namespace phatbooks

#endif  // GUARD_account_handle_hpp_7530966385980586
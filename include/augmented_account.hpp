#ifndef GUARD_augmented_account_hpp_15782473057407176
#define GUARD_augmented_account_hpp_15782473057407176

#include "account.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>

namespace phatbooks
{

class Commodity;
class PhatbooksDatabaseConnection;

/**
 * Represents an Account, together with an amount
 * representing its opening balance. This is simply a convenient way of
 * grouping these two pieces of information together - hence a
 * struct.
 */
struct AugmentedAccount
{
	AugmentedAccount
	(	PhatbooksDatabaseConnection& p_database_connection,
		sqloxx::Handle<Commodity> const& p_commodity
	);
	AugmentedAccount
	(	sqloxx::Handle<Account> const& p_account,
		jewel::Decimal const& p_technical_opening_balance
	);
	sqloxx::Handle<Account> account;
	jewel::Decimal technical_opening_balance;

};  // struct AugmentedAccount

}  // namespace phatbooks

#endif  // GUARD_augmented_account_hpp_15782473057407176

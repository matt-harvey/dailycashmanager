// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "augmented_account.hpp"
#include "account.hpp"
#include "commodity_handle.hpp"
#include "phatbooks_database_connection.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>

using jewel::Decimal;
using sqloxx::Handle;

namespace phatbooks
{


AugmentedAccount::AugmentedAccount
(	PhatbooksDatabaseConnection& p_database_connection,
	CommodityHandle const& p_commodity
):
	account(p_database_connection),
	technical_opening_balance(0, p_commodity->precision())
{
}

AugmentedAccount::AugmentedAccount
(	Handle<Account> const& p_account,
	jewel::Decimal const& p_technical_opening_balance
):
	account(p_account),
	technical_opening_balance(p_technical_opening_balance)
{
}

}   // namespace phatbooks

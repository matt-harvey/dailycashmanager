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

#include "augmented_account.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>

using jewel::Decimal;
using sqloxx::Handle;

namespace phatbooks
{


AugmentedAccount::AugmentedAccount
(	PhatbooksDatabaseConnection& p_database_connection,
	Handle<Commodity> const& p_commodity
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

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

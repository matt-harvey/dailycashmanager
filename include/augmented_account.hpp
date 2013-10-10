/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

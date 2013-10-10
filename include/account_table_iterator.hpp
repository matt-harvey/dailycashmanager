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


#ifndef GUARD_account_table_iterator_hpp_048275898233184546
#define GUARD_account_table_iterator_hpp_048275898233184546

#include "account.hpp"
#include <sqloxx/handle.hpp>
#include <sqloxx/table_iterator.hpp>

namespace phatbooks
{

typedef
	sqloxx::TableIterator<sqloxx::Handle<Account> >
	AccountTableIterator;

/**
 * @returns an AccountTableIterator that iterates over all Accounts
 * ordered by name.
 */
AccountTableIterator
make_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

/**
 * @returns an AccountTableIterator that iterates over all Accounts,
 * ordered first by AccountType, then by name.
 */
AccountTableIterator
make_type_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
);

}  // namespace phatbooks

#endif  // GUARD_account_table_iterator_hpp_048275898233184546

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

#ifndef GUARD_make_default_accounts_hpp_29520491757206846
#define GUARD_make_default_accounts_hpp_29520491757206846

#include "account_type.hpp"
#include <sqloxx/handle_fwd.hpp>
#include <vector>

namespace dcm
{

// begin forward declarations

class Account;
class DcmDatabaseConnection;

// end forward declarations

/**
 * @returns a vector of handles to "suggested default Accounts" that might be
 * presented to the user as a starting point when setting up a new DCM
 * file. The Accounts will all names and AccountTypes initialized, and will 
 * have an empty wxString as their description. They will \e not have their
 * Commodity initialized though - this must be done before the Accounts
 * are saved.
 *
 * @param p_database_connection a connection to the database with which
 * the returned sqloxx::Handle<Account> instances will be associated
 * (but note, calling this function will \e not cause the Accounts to be
 * saved to the database).
 */
std::vector<sqloxx::Handle<Account> >
make_default_accounts(DcmDatabaseConnection& p_database_connection);


/**
 * Behaves like make_default_accounts(DcmDatabaseConnection&), but
 * instead of returning a vector, it populates the vector passed to \e vec.
 *
 * @param vec the vector which the function will populate with
 * sqloxx::Handle<Account>, which will be pushed onto the back of
 * e\ vec - which need not be empty when passed to the function.
 */
void make_default_accounts
(	DcmDatabaseConnection& p_database_connection,
	std::vector<sqloxx::Handle<Account> >& vec
);

/**
 * Behaves like the two-parameter function with the same name, but only
 * populates \e vec with default Accounts of AccountType p_account_type.
 * The sqloxx::Handle<Account> will be pushed onto the back of \e vec - which
 * need not be empty when passed to the function.
 */
void make_default_accounts
(	DcmDatabaseConnection& p_database_connection,
	std::vector<sqloxx::Handle<Account> >& vec,
	AccountType p_account_type
);


}  // namespace dcm

#endif  // GUARD_make_default_accounts_hpp_29520491757206846

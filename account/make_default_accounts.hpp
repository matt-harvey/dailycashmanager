// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_make_default_accounts_hpp_29520491757206846
#define GUARD_make_default_accounts_hpp_29520491757206846

#include "account_handle.hpp"
#include "account_type.hpp"
#include <vector>

namespace phatbooks
{

class PhatbooksDatabaseConnection;


/**
 * @returns a vector of handles to "suggested default Accounts" that might be
 * presented to the user as a starting point when setting up a new Phatbooks
 * file. The Accounts will all names and AccountTypes initialized, and will 
 * have an empty wxString as their description. They will \e not have their
 * Commodity initialized though - this must be done before the Accounts
 * are saved.
 *
 * @param p_database_connection a connection to the database with which
 * the returned AccountHandles will be associated (but note, calling this
 * function will \e not cause the Accounts to be saved to the database).
 */
std::vector<AccountHandle>
make_default_accounts(PhatbooksDatabaseConnection& p_database_connection);


/**
 * Behaves like make_default_accounts(PhatbooksDatabaseConnection&), but
 * instead of returning a vector, it populates the vector passed to \e vec.
 *
 * @param vec the vector which the function will populate with AccountHandles. The
 * AccountHandles will be pushed onto the back of e\ vec - which need not be empty
 * when passed to the function.
 */
void make_default_accounts
(	PhatbooksDatabaseConnection& p_database_connection,
	std::vector<AccountHandle>& vec
);

/**
 * Behaves like the two-parameter function with the same name, but only
 * populates \e vec with default Accounts of AccountType p_account_type.
 * The AccountHandles will be pushed onto the back of \e vec - which need not be
 * empty when passed to the function.
 */
void make_default_accounts
(	PhatbooksDatabaseConnection& p_database_connection,
	std::vector<AccountHandle>& vec,
	AccountType p_account_type
);


}  // namespace phatbooks

#endif  // GUARD_make_default_accounts_hpp_29520491757206846

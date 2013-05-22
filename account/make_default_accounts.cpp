// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "make_default_accounts.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include <cassert>
#include <utility>
#include <vector>

using std::make_pair;
using std::pair;
using std::vector;

namespace phatbooks
{

vector<Account>
make_default_accounts(PhatbooksDatabaseConnection& p_database_connection)
{
	vector<Account> ret;
	make_default_accounts(p_database_connection, ret);
	return ret;
}

void
make_default_accounts
(	PhatbooksDatabaseConnection& p_database_connection,
	vector<Account>& vec
)
{
	typedef vector<account_type::AccountType> Vec;
	Vec const atypes = account_types();
	for (Vec::const_iterator it = atypes.begin(); it != atypes.end(); ++it)
	{
		make_default_accounts(p_database_connection, vec, *it);
	}
	return;
}

void
make_default_accounts
(	PhatbooksDatabaseConnection& p_database_connection,
	vector<Account>& vec,
	account_type::AccountType p_account_type
)
{
	vector<BString> names;

	// First we fill a vector with the Account names.
	switch (p_account_type)
	{
	case account_type::asset:
		names.push_back(BString("Cash"));
		names.push_back(BString("Cheque account"));
		break;
	case account_type::liability:
		names.push_back(BString("Credit card"));
		break;
	case account_type::equity:
		// There are no default equity Accounts.
		break;
	case account_type::revenue:
		names.push_back(BString("Salary"));
		names.push_back(BString("Interest received"));
		break;
	case account_type::expense:
		names.reserve(7);
		names.push_back(BString("Food"));
		names.push_back(BString("Household supplies"));
		names.push_back(BString("Rent"));
		names.push_back(BString("Electricity"));
		names.push_back(BString("Gas"));
		names.push_back(BString("Interest paid"));
		names.push_back(BString("Recreation"));
		// TODO Do we want a "mortgage" Account among the defaults?
		// The correct accounting for mortgage repayments is fairly complex.
		// How would we present it to the user? Would we just cheat and treat
		// it as an expense?
		break;
	case account_type::pure_envelope:
		// There are no default pure_envelope Accounts.
		break;
	default:
		assert (false);
	}
	// Now we use this information to populate vec with actual Accounts
	// (but note we don't save them - saving them will be at the discretion
	// of the user, and will be done in client code closer to the UI).
	for (vector<BString>::size_type i = 0; i != names.size(); ++i)
	{
		Account account(p_database_connection);
		account.set_name(names[i]);
		account.set_account_type(p_account_type);

		// Note Commodity is left uninitialized. This is a bit odd.
		// Previously we set the Commodity for all these Accounts to
		// m_database_connection.default_commodity(). But it was
		// discovered this creates headaches for client code, as
		// we then can't create these Accounts unless
		// the default Commodity has already been set for this
		// PhatbooksDatabaseConnection. It is easier for certain client code
		// if it is able to create the Accounts first.

		account.set_description(BString(""));
		vec.push_back(account);
	}
	return;
}



}  // namespace phatbooks

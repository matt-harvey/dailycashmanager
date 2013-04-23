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
	assert (vec.empty());  // precondition

	// First we fill a vector with the minimum information we are going to
	// need about each Account.
	using account_type::AccountType;
	using account_type::asset;
	using account_type::liability;
	using account_type::revenue;
	using account_type::expense;
	using account_type::pure_envelope;
	typedef pair<BString, account_type::AccountType> ProtoAccount;
	typedef vector<ProtoAccount> ProtoVec;
	typedef AccountType AT;
	typedef BString BS;
	ProtoVec pv;
	pv.reserve(12);
	pv.push_back(make_pair<BS, AT>("Cash", asset));
	pv.push_back(make_pair<BS, AT>("Cheque account", asset));
	pv.push_back(make_pair<BS, AT>("Credit card", liability));
	pv.push_back(make_pair<BS, AT>("Salary", revenue));
	pv.push_back(make_pair<BS, AT>("Interest earned", revenue));
	pv.push_back(make_pair<BS, AT>("Food", expense));
	pv.push_back(make_pair<BS, AT>("Household supplies", expense));
	pv.push_back(make_pair<BS, AT>("Rent", expense));
	pv.push_back(make_pair<BS, AT>("Electricity", expense));
	pv.push_back(make_pair<BS, AT>("Gas", expense));
	pv.push_back(make_pair<BS, AT>("Interest paid", expense));
	pv.push_back(make_pair<BS, AT>("Recreation", expense));

		// TODO Do we want a "mortgage" Account among the defaults?
		// The correct accounting for mortgage repayments is fairly complex.
		// How would we present it to the user? Would we just cheat and treat
		// it as an expense?

	// Now we use this information to populate m_accounts with actual Accounts
	// (but note we don't save them - saving them will be at the discretion
	// of the user, and will be done in client code closer to the UI).

	vector<ProtoAccount>::size_type sz = pv.size();
	vec.reserve(sz);	
	for (vector<ProtoAccount>::size_type i = 0; i != sz; ++i)
	{
		Account account(p_database_connection);
		account.set_name(pv[i].first);
		account.set_account_type(pv[i].second);

		// Note Commodity is left uninitialized! This is a bit odd.
		// Previously we set the Commodity for all these Accounts to
		// m_database_connection.default_commodity(). But it was
		// discovered this creates headaches for client code, as
		// we then can't create these Accounts unless
		// the default Commodity has already been set for this
		// PhatbooksDatabaseConnection. It is easier for certain client code
		// if it is able to create the Accounts first.

		account.set_description("");
		vec.push_back(account);
	}

	assert (vec.size() == pv.size());
	assert (vec.size() != 0);

	return;
}




}  // namespace phatbooks

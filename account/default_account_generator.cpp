#include "default_account_generator.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/database_transaction.hpp>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>

using sqloxx::DatabaseTransaction;
using std::exception;
using std::make_pair;
using std::pair;
using std::vector;

namespace phatbooks
{

DefaultAccountGenerator::DefaultAccountGenerator
(	PhatbooksDatabaseConnection& p_phatbooks_database_connection
):
	m_accounts(0),
	m_database_connection(p_phatbooks_database_connection)
{
}

DefaultAccountGenerator::~DefaultAccountGenerator()
{
	delete m_accounts;
	m_accounts = 0;
}

vector<Account>&
DefaultAccountGenerator::accounts()
{
	if (!m_accounts)
	{
		m_accounts = new vector<Account>;
		initialize_default_accounts();
	}
	assert (m_accounts);
	return *m_accounts;
}

void
DefaultAccountGenerator::save_accounts()
{
	DatabaseTransaction transaction(m_database_connection);
	try
	{
		for
		(	vector<Account>::iterator it = m_accounts->begin(),
				end = m_accounts->end();
			it != end;
			++it
		)
		{
			it->save();
		}
		transaction.commit();
	}
	catch (exception&)
	{
		transaction.cancel();
		for
		(	vector<Account>::iterator it = m_accounts->begin(),
				end = m_accounts->end();
			it != end;
			++it
		)
		{
			it->ghostify();
		}
	}
	return;
}

void
DefaultAccountGenerator::initialize_default_accounts()
{
	// TODO This assumes we have PHATBOOKS_EXPOSE_COMMODITY
	// switched off. So this is "unconditional compilation", whereas
	// elsewhere we have conditional compilation for this same
	// switch. Make this consistent.

	// Assert preconditions

	assert (m_accounts != 0);
	assert (m_accounts->empty());

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
	m_accounts->reserve(sz);
	for (vector<ProtoAccount>::size_type i = 0; i != sz; ++i)
	{
		Account account(m_database_connection);
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
		m_accounts->push_back(account);
	}

	assert (m_accounts->size() == pv.size());
	assert (m_accounts->size() != 0);

	return;
}




}  // namespace phatbooks
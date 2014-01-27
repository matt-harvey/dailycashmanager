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

#include "make_default_accounts.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "dcm_database_connection.hpp"
#include "visibility.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <utility>
#include <vector>

using sqloxx::Handle;
using std::make_pair;
using std::pair;
using std::vector;

namespace dcm
{

vector<Handle<Account> >
make_default_accounts(DcmDatabaseConnection& p_database_connection)
{
	vector<Handle<Account> > ret;
	make_default_accounts(p_database_connection, ret);
	return ret;
}

void
make_default_accounts
(	DcmDatabaseConnection& p_database_connection,
	vector<Handle<Account> >& vec
)
{
	for (AccountType atype: account_types())
	{
		make_default_accounts(p_database_connection, vec, atype);
	}
	return;
}

void
make_default_accounts
(	DcmDatabaseConnection& p_database_connection,
	vector<Handle<Account> >& vec,
	AccountType p_account_type
)
{
	struct ProtoAccount
	{
		wxString const name;
		wxString const description;
	};

	vector<ProtoAccount> proto_accounts;

	switch (p_account_type)
	{
	case AccountType::asset:
		proto_accounts = vector<ProtoAccount>
		{	ProtoAccount{"Cash", "Notes and coins"},
			ProtoAccount{"Cheque account", ""}
		};
		break;
	case AccountType::liability:
		proto_accounts = vector<ProtoAccount>
		{	ProtoAccount{"Credit card", ""}
		};
		break;
	case AccountType::equity:
		// There are no default equity Accounts.
		break;
	case AccountType::revenue:
		proto_accounts = vector<ProtoAccount>
		{	ProtoAccount{"Salary", ""},
			ProtoAccount{"Interest received", ""}
		};
		break;
	case AccountType::expense:
		proto_accounts = vector<ProtoAccount>
		{	ProtoAccount{"Food", "Food and drink"},
			ProtoAccount
			{	"Household supplies",
				"Small household consumables, excluding food"
			},
			ProtoAccount{"Rent", ""},
			ProtoAccount{"Electricity", ""},
			ProtoAccount{"Interest paid", ""},
			ProtoAccount{"Entertainment", "Books, music, films etc."}
		};
		// TODO MEDIUM PRIORITY Do we want a "mortgage" Account among the
		// defaults? The correct accounting for mortgage repayments is fairly
		// complex. How would we present it to the user? Would we just cheat and
		// treat it as an expense?
		break;
	case AccountType::pure_envelope:
		// There are no default pure_envelope Accounts.
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	// Now we use this information to populate vec with actual
	// Handle<Account> instances
	// (but note we don't save the Accounts - saving them will be at the
	// discretion of the user, and will be done in client code closer to
	// the UI).
	for (auto const& proto_account: proto_accounts)
	{
		Handle<Account> const account(p_database_connection);
		account->set_name(proto_account.name);
		account->set_account_type(p_account_type);

		// Note Commodity is left uninitialized. This is a bit odd.
		// Previously we set the Commodity for all these Accounts to
		// m_database_connection.default_commodity(). But it was
		// discovered this creates headaches for client code, as
		// we then can't create these Accounts unless
		// the default Commodity has already been set for this
		// DcmDatabaseConnection. It is easier for certain client code
		// if it is able to create the Accounts first.

		account->set_description(proto_account.description);
		account->set_visibility(Visibility::visible);
		vec.push_back(account);
	}
	return;
}



}  // namespace dcm

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


#include "make_default_accounts.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "phatbooks_database_connection.hpp"
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

namespace phatbooks
{

vector<Handle<Account> >
make_default_accounts(PhatbooksDatabaseConnection& p_database_connection)
{
	vector<Handle<Account> > ret;
	make_default_accounts(p_database_connection, ret);
	return ret;
}

void
make_default_accounts
(	PhatbooksDatabaseConnection& p_database_connection,
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
(	PhatbooksDatabaseConnection& p_database_connection,
	vector<Handle<Account> >& vec,
	AccountType p_account_type
)
{
	vector<wxString> names;

	// First we fill a vector with the Account names.
	switch (p_account_type)
	{
	case AccountType::asset:
		names.push_back(wxString("Cash"));
		names.push_back(wxString("Cheque account"));
		break;
	case AccountType::liability:
		names.push_back(wxString("Credit card"));
		break;
	case AccountType::equity:
		// There are no default equity Accounts.
		break;
	case AccountType::revenue:
		names.push_back(wxString("Salary"));
		names.push_back(wxString("Interest received"));
		break;
	case AccountType::expense:
		names.reserve(7);
		names.push_back(wxString("Food"));
		names.push_back(wxString("Household supplies"));
		names.push_back(wxString("Rent"));
		names.push_back(wxString("Electricity"));
		names.push_back(wxString("Gas"));
		names.push_back(wxString("Interest paid"));
		names.push_back(wxString("Recreation"));
		// TODO Do we want a "mortgage" Account among the defaults?
		// The correct accounting for mortgage repayments is fairly complex.
		// How would we present it to the user? Would we just cheat and treat
		// it as an expense?
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
	for (wxString const& name: names)
	{
		Handle<Account> const account(p_database_connection);
		account->set_name(name);
		account->set_account_type(p_account_type);

		// Note Commodity is left uninitialized. This is a bit odd.
		// Previously we set the Commodity for all these Accounts to
		// m_database_connection.default_commodity(). But it was
		// discovered this creates headaches for client code, as
		// we then can't create these Accounts unless
		// the default Commodity has already been set for this
		// PhatbooksDatabaseConnection. It is easier for certain client code
		// if it is able to create the Accounts first.

		account->set_description(wxString(""));
		account->set_visibility(Visibility::visible);
		vec.push_back(account);
	}
	return;
}



}  // namespace phatbooks

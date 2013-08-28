// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account.hpp"
#include "account_type.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "entry_reader.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "b_string.hpp"
#include <boost/shared_ptr.hpp>
#include <consolixx/alignment.hpp>
#include <consolixx/column.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <map>
#include <string>
#include <vector>

using consolixx::AccumulatingColumn;
using consolixx::PlainColumn;
using phatbooks::account_type::AccountType;
using phatbooks::account_super_type::AccountSuperType;
using sqloxx::Handle;
using boost::shared_ptr;
using jewel::Decimal;
using std::map;
using std::string;
using std::vector;

// for debugging
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


namespace alignment = consolixx::alignment;

namespace phatbooks
{

namespace
{
	/**
	 * Populates \e out with data indicating, for each Account::Id, the
	 * number of <em>actual, ordinary</em> Entries using the corresponding
	 * Account.
	 *
	 * @todo Could have a potentially more efficient version of this function
	 * which instead of taking a PhatbooksDatabaseConnection&, takes a pair
	 * of Entry iterators (which could then be re-used from an existing
	 * ActualOrdinaryEntryReader).
	 */
	void actual_account_usage_map
	(	PhatbooksDatabaseConnection& p_database_connection,
		map<Account::Id, size_t>& out
	)
	{
		AccountReader const a_reader(p_database_connection);
		AccountReader::const_iterator a_it = a_reader.begin();
		AccountReader::const_iterator const a_end = a_reader.end();
		for ( ; a_it != a_end; ++a_it)
		{
			out[a_it->id()] = 0;
		}

		ActualOrdinaryEntryReader const e_reader(p_database_connection);
		ActualOrdinaryEntryReader::const_iterator e_it = e_reader.begin();
		ActualOrdinaryEntryReader::const_iterator const e_end = e_reader.end();
		for ( ; e_it != e_end; ++e_it)
		{
			++out[e_it->account().id()];
		}

		return;
	}

}  // end anonymous namespace


void
Account::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	AccountImpl::setup_tables(dbc);
	return;
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection
):
	PhatbooksPersistentObject(p_database_connection)
{
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}

Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	BString const& p_name
):
	PhatbooksPersistentObject
	(	p_database_connection,
		AccountImpl::id_for_name(p_database_connection, p_name)
	)
{
}

Account::Account(sqloxx::Handle<AccountImpl> const& p_handle):
	PhatbooksPersistentObject(p_handle)
{
}

Account
Account::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Account
	(	sqloxx::Handle<AccountImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}

Account::~Account()
{
}


bool
Account::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	BString const& p_name
)
{
	return AccountImpl::exists
	(	p_database_connection,
		p_name
	);
}

bool
Account::no_user_pl_accounts_saved
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return AccountImpl::no_user_pl_accounts_saved
	(	p_database_connection
	);
}

bool
Account::none_saved_with_account_type
(	PhatbooksDatabaseConnection& p_database_connection,
	account_type::AccountType p_account_type
)
{
	return AccountImpl::none_saved_with_account_type
	(	p_database_connection,
		p_account_type
	);
}

bool
Account::none_saved_with_account_super_type
(	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type
)
{
	return AccountImpl::none_saved_with_account_super_type
	(	p_database_connection,
		p_account_super_type
	);
}


BString
Account::name() const
{
	return impl().name();
}

Commodity
Account::commodity() const
{
	return impl().commodity();
}

AccountType
Account::account_type() const
{
	return impl().account_type();
}

AccountSuperType
Account::account_super_type() const
{
	return impl().account_super_type();
}

BString
Account::description() const
{
	return impl().description();
}

Decimal
Account::budget() const
{
	return impl().budget();
}

vector<BudgetItem>
Account::budget_items() const
{
	return impl().budget_items();
}

Decimal
Account::technical_balance() const
{
	return impl().technical_balance();
}

Decimal
Account::friendly_balance() const
{
	return impl().friendly_balance();
}

Decimal
Account::technical_opening_balance() const
{
	return impl().technical_opening_balance();
}

Decimal
Account::friendly_opening_balance() const
{
	return impl().friendly_opening_balance();
}

void
Account::set_account_type(AccountType p_account_type)
{
	impl().set_account_type(p_account_type);
	return;
}

void
Account::set_name(BString const& p_name)
{
	impl().set_name(p_name);
	return;
}

void
Account::set_commodity(Commodity const& p_commodity)
{
	impl().set_commodity(p_commodity);
	return;
}

void
Account::set_description(BString const& p_description)
{
	impl().set_description(p_description);
	return;
}

// AugmentedAccount


AugmentedAccount::AugmentedAccount
(	PhatbooksDatabaseConnection& p_database_connection,
	Commodity const& p_commodity
):
	account(p_database_connection),
	technical_opening_balance(0, p_commodity.precision())
{
}

AugmentedAccount::AugmentedAccount
(	Account const& p_account,
	jewel::Decimal const& p_technical_opening_balance
):
	account(p_account),
	technical_opening_balance(p_technical_opening_balance)
{
}


// Free-standing functions

bool is_asset_or_liability(Account const& account)
{
	switch (account.account_type())
	{
	case account_type::asset:
	case account_type::liability:
		return true;
	default:
		return false;
	}
}

bool is_balance_sheet_account(Account const& account)
{
	return account.account_super_type() ==
		account_super_type::balance_sheet;
}

bool is_expense(Account const& account)
{
	return account.account_type() == account_type::expense;
}

bool is_revenue(Account const& account)
{
	return account.account_type() == account_type::revenue;
}

bool is_pl_account(Account const& account)
{
	return account.account_super_type() == account_super_type::pl;
}


bool is_not_pure_envelope(Account const& account)
{
	return account.account_type() != account_type::pure_envelope;
}

BString account_concept_name
(	account_super_type::AccountSuperType p_account_super_type,
	bool p_capitalize,
	bool p_include_article
)
{
	BString ret;
	assert (ret.IsEmpty());
	switch (p_account_super_type)
	{
	case account_super_type::balance_sheet:
		if (p_include_article) ret += BString("an ");
		ret += (p_capitalize? BString("Account"): BString("account"));
		break;
	case account_super_type::pl:
		if (p_include_article) ret += BString("an ");
		ret += (p_capitalize? BString("Envelope"): BString("envelope"));
		break;
	default:
		assert (false);
	}
	return ret;
}

BString account_concepts_phrase(bool p_include_article)
{
	BString const ret =
		account_concept_name
		(	account_super_type::balance_sheet,
			false,
			p_include_article
		) +
		BString(" or ") +
		account_concept_name(account_super_type::pl);
	return ret;
}

map<account_super_type::AccountSuperType, Account::Id>
favourite_accounts(PhatbooksDatabaseConnection& p_database_connection)
{
	map<account_super_type::AccountSuperType, Account::Id> ret;
	map<account_super_type::AccountSuperType, size_t> max_counts;
	map<Account::Id, size_t> account_map;
	actual_account_usage_map(p_database_connection, account_map);
	vector<account_super_type::AccountSuperType> const& super_types =
		account_super_types();
	for
	(	vector<account_super_type::AccountSuperType>::size_type i = 0;
		i != super_types.size();
		++i
	)
	{
		max_counts[super_types[i]] = 0;
	}
	map<Account::Id, size_t>::const_iterator it = account_map.begin();
	map<Account::Id, size_t>::const_iterator const end = account_map.end();
	Account const balancing_acct = p_database_connection.balancing_account();
	for ( ; it != end; ++it)
	{
		Account const account(p_database_connection, it->first);
		size_t const count = it->second;
		account_super_type::AccountSuperType const stype =
			super_type(account.account_type());
		if
		(	(	(account_map[account.id()] >= max_counts[stype]) ||
				(ret[stype] == balancing_acct.id())
			)
			&&
			(	(account != balancing_acct)
			)
		)
		{
			assert (account.has_id());
			assert (it->first == account.id());
			ret[stype] = account.id();
			max_counts[stype] = count;
		}
	}
	return ret;
}

}   // namespace phatbooks

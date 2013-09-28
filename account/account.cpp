// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account.hpp"
#include "account_type.hpp"
#include "account_impl.hpp"
#include "account_table_iterator.hpp"
#include "date.hpp"
#include "string_conv.hpp"
#include "commodity.hpp"
#include "entry_table_iterator.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include "string_flags.hpp"
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <map>
#include <string>
#include <vector>

using phatbooks::account_type::AccountType;
using phatbooks::account_super_type::AccountSuperType;
using sqloxx::Handle;
using jewel::Decimal;
using std::map;
using std::string;
using std::vector;


namespace phatbooks
{

void
Account::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	JEWEL_LOG_TRACE();
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
	wxString const& p_name
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

bool
Account::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
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


wxString
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

wxString
Account::description() const
{
	return impl().description();
}

visibility::Visibility
Account::visibility() const
{
	return impl().visibility();
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
Account::set_name(wxString const& p_name)
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
Account::set_description(wxString const& p_description)
{
	impl().set_description(p_description);
	return;
}

void
Account::set_visibility(visibility::Visibility p_visibility)
{
	impl().set_visibility(p_visibility);
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

wxString account_concept_name
(	account_super_type::AccountSuperType p_account_super_type,
	AccountPhraseFlags p_phrase_flag_set
)
{
	wxString ret;
	JEWEL_ASSERT (ret.IsEmpty());
	if (p_phrase_flag_set.test(string_flags::include_article))
	{
		ret += wxString("an ");
	}
	bool const capitalize = p_phrase_flag_set.test(string_flags::capitalize);
	switch (p_account_super_type)
	{
	case account_super_type::balance_sheet:
		ret +=
		(	capitalize?
			wxString("Account"):
			wxString("account")
		);
		break;
	case account_super_type::pl:
		ret +=
		(	capitalize?
			wxString("Envelope"):
			wxString("envelope")
		);
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	if (p_phrase_flag_set.test(string_flags::pluralize))
	{
		ret += wxString("s");
	}
	return ret;
}

wxString account_concepts_phrase
(	AccountPhraseFlags p_phrase_flag_set
)
{
	wxString ret = account_concept_name
	(	account_super_type::balance_sheet,
		p_phrase_flag_set
	);
	ret += wxString(" or ");
	p_phrase_flag_set.clear(string_flags::include_article);
	ret += account_concept_name
	(	account_super_type::pl,
		p_phrase_flag_set
	);
	return ret;
}

map<account_super_type::AccountSuperType, Account::Id>
favourite_accounts(PhatbooksDatabaseConnection& p_database_connection)
{
	map<account_super_type::AccountSuperType, Account::Id> ret;
	favourite_accounts(p_database_connection, ret);
	return ret;
}

}   // namespace phatbooks

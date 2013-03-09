#include "account.hpp"
#include "account_type.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "b_string.hpp"
#include <boost/shared_ptr.hpp>
#include <consolixx/alignment.hpp>
#include <consolixx/column.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <string>
#include <vector>

using consolixx::AccumulatingColumn;
using consolixx::PlainColumn;
using phatbooks::account_type::AccountType;
using sqloxx::Handle;
using boost::shared_ptr;
using jewel::Decimal;
using std::string;
using std::vector;

namespace alignment = consolixx::alignment;


namespace phatbooks
{



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

BString
Account::description() const
{
	return impl().description();
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


namespace
{
	// Convenient non-member functions for initializing consolixx::Columns.
	BString col_aux_name(Account const& account)
	{
		return account.name();
	}
	account_type::AccountType col_aux_account_type(Account const& account)
	{
		return account.account_type();
	}
	string col_aux_account_type_to_std8(account_type::AccountType type)
	{
		return bstring_to_std8(account_type_to_string(type));
	}
	BString col_aux_description(Account const& account)
	{
		return account.description();
	}
	Decimal col_aux_friendly_balance(Account const& account)
	{
		return account.friendly_balance();
	}
	Decimal col_aux_accumulating_friendly_balance
	(	Account const& account,
		Decimal& accumulator
	)
	{
		Decimal const ret = account.friendly_balance();
		accumulator += ret;
		return ret;
	}
}  // end anonymous namespace



PlainColumn<Account, BString>*
Account::create_name_column()
{
	return new PlainColumn<Account, BString>
	(	col_aux_name,
		"Account",
		alignment::left,
		bstring_to_std8
	);
}

PlainColumn<Account, account_type::AccountType>*
Account::create_type_column()
{
	return new PlainColumn<Account, account_type::AccountType>
	(	col_aux_account_type,
		"Type",
		alignment::left,
		col_aux_account_type_to_std8
	);
}

PlainColumn<Account, BString>*
Account::create_description_column()
{
	return new PlainColumn<Account, BString>
	(	col_aux_description,
		"Description",
		alignment::left,
		bstring_to_std8
	);
}

PlainColumn<Account, Decimal>*
Account::create_friendly_balance_column()
{
	return new PlainColumn<Account, Decimal>
	(	col_aux_friendly_balance,
		"Balance",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Account, Decimal>*
Account::create_accumulating_friendly_balance_column()
{
	return new AccumulatingColumn<Account, Decimal>
	(	col_aux_accumulating_friendly_balance,
		Decimal(0, 0),
		"Balance",
		alignment::right,
		finformat_std8
	);
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

bool is_expense(Account const& account)
{
	return account.account_type() == account_type::expense;
}

bool is_revenue(Account const& account)
{
	return account.account_type() == account_type::revenue;
}

bool is_envelope(Account const& account)
{
	switch (account.account_type())
	{
	case account_type::asset:
	case account_type::liability:
		return false;
	case account_type::equity:
	case account_type::revenue:
	case account_type::expense:
	case account_type::pure_envelope:
		return true;
	default:
		assert (false);	
	}
}


bool is_not_pure_envelope(Account const& account)
{
	return account.account_type() != account_type::pure_envelope;
}



}   // namespace phatbooks

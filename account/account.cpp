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
using phatbooks::account_super_type::AccountSuperType;
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
Account::technical_balance() const
{
	return impl().technical_balance();
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



}   // namespace phatbooks

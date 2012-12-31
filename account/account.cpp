#include "account.hpp"
#include "account_type.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "commodity.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>
#include <vector>

using phatbooks::account_type::AccountType;
using sqloxx::Handle;
using boost::shared_ptr;
using jewel::Decimal;
using std::string;
using std::vector;


namespace phatbooks
{

std::vector<std::string>
Account::account_type_names()
{
	return AccountImpl::account_type_names();
}

void
Account::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	AccountImpl::setup_tables(dbc);
	return;
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_impl(Handle<AccountImpl>(p_database_connection))
{
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl
	(	Handle<AccountImpl>(p_database_connection, p_id)
	)
{
}

Account
Account::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Account
	(	Handle<AccountImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}
	

Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	string const& p_name
):
	m_impl
	(	Handle<AccountImpl>
		(	p_database_connection,
			AccountImpl::id_for_name(p_database_connection, p_name)
		)
	)
{
}


Account::~Account()
{
}

// WARNING This should eventually be shifted into a base class.
Account::Id
Account::id() const
{
	return m_impl->id();
}

// WARNING This should eventually be shifted into a base class.
void
Account::save()
{
	m_impl->save();
	return;
}

std::string
Account::name() const
{
	return m_impl->name();
}

Commodity
Account::commodity() const
{
	return m_impl->commodity();
}

AccountType
Account::account_type() const
{
	return m_impl->account_type();
}

std::string
Account::description() const
{
	return m_impl->description();
}

Decimal
Account::technical_balance() const
{
	return m_impl->technical_balance();
}

Decimal
Account::friendly_balance() const
{
	return m_impl->friendly_balance();
}

void
Account::set_account_type(AccountType p_account_type)
{
	m_impl->set_account_type(p_account_type);
	return;
}

void
Account::set_name(string const& p_name)
{
	m_impl->set_name(p_name);
	return;
}

void
Account::set_commodity(Commodity const& p_commodity)
{
	m_impl->set_commodity(p_commodity);
	return;
}

void
Account::set_description(std::string const& p_description)
{
	m_impl->set_description(p_description);
	return;
}

Account::Account(sqloxx::Handle<AccountImpl> const& p_handle):
	m_impl(p_handle)
{
}


shared_ptr<vector<string> >
make_account_row(Account const& account)
{
	shared_ptr<vector<string> > ret(new vector<string>);
	ret->push_back(account.name());
	ret->push_back(finformat(account.friendly_balance()));
	return ret;
}



// Free-standing functions

bool is_asset_or_liability(Account const& account)
{
	bool ret;
	switch (account.account_type())
	{
	case account_type::asset:
	case account_type::liability:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}
	return ret;
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
	bool ret;
	switch (account.account_type())
	{
	case account_type::asset:
	case account_type::liability:
		ret = false;
		break;
	case account_type::equity:
	case account_type::revenue:
	case account_type::expense:
	case account_type::pure_envelope:
		ret = true;
		break;
	default:
		assert (false);	
	}
	return ret;
}


}   // namespace phatbooks

#include "account.hpp"
#include "account_type.hpp"
#include "account_impl.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using phatbooks::account_type::AccountType;
using boost::shared_ptr;
using sqloxx::get_handle;
using std::string;


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
	m_impl(get_handle<AccountImpl>(p_database_connection))
{
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	m_impl
	(	get_handle<AccountImpl>(p_database_connection, p_id)
	)
{
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	string const& p_name
):
	m_impl
	(	get_handle<AccountImpl>
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



}   // namespace phatbooks

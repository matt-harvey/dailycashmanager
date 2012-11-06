/** \file account.cpp
 *
 * \brief Source file for code pertaining to Account class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "account.hpp"
#include "account_impl.hpp"
#include "commodity.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <string>
#include <vector>

using boost::shared_ptr;
using sqloxx::DatabaseConnection;
using std::string;

namespace phatbooks
{

std::vector<std::string>
Account::account_type_names()
{
	return AccountImpl::account_type_names();
}

void
Account::setup_tables(sqloxx::DatabaseConnection& dbc)
{
	AccountImpl::setup_tables(dbc);
	return;
}

Account::Account(shared_ptr<DatabaseConnection> p_database_connection):
	PersistentObject(p_database_connection),
	m_impl(new AccountImpl(p_database_connection))
{
}

Account::Account
(	shared_ptr<DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_impl(new AccountImpl(p_database_connection, p_id))
{
}

Account::Account
(	shared_ptr<DatabaseConnection> p_database_connection,
	string const& p_name
):
	PersistentObject(p_database_connection),
	m_impl(new AccountImpl(p_database_connection, p_name))
{
}

string
Account::name()
{
	return m_impl->name();
}

string
Account::commodity_abbreviation()
{
	return m_impl->commodity_abbreviation();
}

Account::AccountType
Account::account_type()
{
	return m_impl->account_type();
}

string
Account::description()
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
Account::set_commodity_abbreviation(string const& p_commodity_abbreviation)
{
	m_impl->set_commodity_abbreviation(p_commodity_abbreviation);
	return;
}

void
Account::set_description(string const& p_description)
{
	m_impl->set_description(p_description);
	return;
}

void
Account::do_load_all()
{
	m_impl->do_load_all();
	return;
}

/*
void
Account::do_save_existing_all()
{
	m_impl->do_save_existing_all();
	return;
}

void
Account::do_save_existing_partial()
{
	m_impl->do_save_existing_partial();
	return;
}
*/

void
Account::do_save_new_all()
{
	m_impl->do_save_new_all();
	return;
}

string
Account::do_get_table_name() const
{
	return m_impl->do_get_table_name();
}

void
Account::load_name_knowing_id()
{
	m_impl->load_name_knowing_id();
	return;
}

void
Account::load_id_knowing_name()
{
	m_impl->load_id_knowing_name();
	return;
}




}  // namespace phatbooks

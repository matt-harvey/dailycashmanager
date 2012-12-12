/** \file account_impl.cpp
 *
 * \brief Source file for code pertaining to AccountImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "account.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/identity_map.hpp"
#include "sqloxx/sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using boost::shared_ptr;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using sqloxx::SQLStatement;
using std::string;
using std::vector;

namespace phatbooks
{

typedef
	PhatbooksDatabaseConnection::BalanceCacheAttorney
	BalanceCacheAttorney;


vector<string>
AccountImpl::account_type_names()
{
	static bool calculated_already = false;
	static vector<string> ret;
	while (!calculated_already)
	{
		ret.push_back("Asset");
		ret.push_back("Liability");
		ret.push_back("Equity");
		ret.push_back("Revenue category");
		ret.push_back("Expense category");
		ret.push_back("Pure envelope");
		calculated_already = true;
	}
	return ret;
}


void
AccountImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table account_types(account_type_id integer primary key "
		"autoincrement, name text not null unique);"
	);
	vector<string> const names = account_type_names();
	for (vector<string>::size_type i = 0; i != names.size(); ++i)
	{
		dbc.execute_sql
		(	"insert into account_types(name) values('" + names[i] + "');"
		);
	}
	dbc.execute_sql
	(	"create table accounts "
		"("
			"account_id integer primary key autoincrement, "
			"account_type_id not null references account_types, "
			"name text not null unique, "
			"description text, "
			"commodity_id references commodities"
		"); "
	);
	dbc.execute_sql
	(	"create unique index account_name_index on accounts(name);"
	);
	return;
}


Account::Id
AccountImpl::id_for_name(PhatbooksDatabaseConnection& dbc, string const& name)
{
	SQLStatement statement
	(	dbc,
		"select account_id from accounts where name = :name"
	);
	statement.bind(":name", name);
	statement.step();
	Id const ret = statement.extract<Id>(0);
	statement.step_final();
	return ret;
}


AccountImpl::AccountImpl
(	IdentityMap& p_identity_map	
):
	PersistentObject(p_identity_map),
	m_data(new AccountData)
{
}


AccountImpl::AccountImpl
(	IdentityMap& p_identity_map,	
	Id p_id
):
	PersistentObject(p_identity_map, p_id),
	m_data(new AccountData)
{
}


AccountImpl::AccountImpl(AccountImpl const& rhs):
	PersistentObject(rhs),
	m_data(new AccountData(*(rhs.m_data)))
{
}


AccountImpl::~AccountImpl()
{
	/* Not necessary if m_data is a smart pointer
	delete m_data;
	m_data = 0;
	*/
}

AccountImpl::AccountType
AccountImpl::account_type()
{
	load();
	return value(m_data->account_type);
}

string
AccountImpl::name()
{
	load();
	return value(m_data->name);
}

Commodity
AccountImpl::commodity()
{
	load();
	return value(m_data->commodity);
}

string
AccountImpl::description()
{
	load();
	return value(m_data->description);
}

Decimal
AccountImpl::balance()
{
	load();  // This may be unnecessary but there's no harm in it.
	return BalanceCacheAttorney::balance
	(	database_connection(),
		id()
	);
}

void
AccountImpl::set_account_type(AccountType p_account_type)
{
	load();
	m_data->account_type = p_account_type;
	return;
}

void
AccountImpl::set_name(string const& p_name)
{
	load();
	m_data->name = p_name;
	return;
}

void
AccountImpl::set_commodity(Commodity const& p_commodity)
{
	load();
	m_data->commodity = p_commodity;
	return;
}

void
AccountImpl::set_description(string const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void
AccountImpl::swap(AccountImpl& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
AccountImpl::do_load()
{
	SQLStatement statement
	(	database_connection(),
		"select name, commodity_id, account_type_id, description "
		"from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	AccountImpl temp(*this);
	temp.m_data->name = statement.extract<string>(0);
	temp.m_data->commodity = Commodity
	(	database_connection(),
		statement.extract<Id>(1)
	);
	temp.m_data->account_type =
		static_cast<AccountType>(statement.extract<int>(2));
	temp.m_data->description = statement.extract<string>(3);
	swap(temp);
	return;
}

void
AccountImpl::process_saving_statement(SQLStatement& statement)
{
	statement.bind
	(	":account_type_id",
		static_cast<int>(value(m_data->account_type))
	);
	statement.bind(":name", value(m_data->name));
	statement.bind(":description", value(m_data->description));
	statement.bind(":commodity_id", value(m_data->commodity).id());
	statement.step_final();
	return;
}

void
AccountImpl::do_save_existing()
{
	BalanceCacheAttorney::mark_as_stale(database_connection());
	SQLStatement updater
	(	database_connection(),
		"update accounts set "
		"name = :name, "
		"commodity_id = :commodity_id, "
		"account_type_id = :account_type_id, "
		"description = :description "
		"where account_id = :account_id"
	);
	updater.bind(":account_id", id());
	process_saving_statement(updater);
	return;
}

void
AccountImpl::do_save_new()
{
	BalanceCacheAttorney::mark_as_stale(database_connection());
	SQLStatement inserter
	(	database_connection(),
		"insert into accounts(account_type_id, name, description, "
		"commodity_id) values(:account_type_id, :name, :description, "
		":commodity_id)"
	);
	process_saving_statement(inserter);
	return;
}

void
AccountImpl::do_remove()
{
	BalanceCacheAttorney::mark_as_stale(database_connection());
	std::string const statement_text =
		"delete from " + primary_table_name() + " where " +
		primary_key_name() + " = :p";
	SQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());
	statement.step_final();
	return;
}


void
AccountImpl::do_ghostify()
{
	// WARNING Is there any situation in which this should
	// mark the balance cache as stale?
	// I don't think so, but if there is,
	// note that marking the balance cache
	// as stale here might perhaps throw an exception (which is contrary
	// to the Sqloxx API which requires do_ghostify() to be
	// non-throwing.
	clear(m_data->name);
	clear(m_data->commodity);
	clear(m_data->account_type);
	clear(m_data->description);
	return;
}


string
AccountImpl::primary_table_name()
{
	return "accounts";
}

string
AccountImpl::primary_key_name()
{
	return "account_id";
}




}  // namespace phatbooks

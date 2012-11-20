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
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using boost::shared_ptr;
using jewel::value;
using sqloxx::SharedSQLStatement;
using std::string;
using std::vector;

namespace phatbooks
{


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
	SharedSQLStatement statement
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
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection
):
	PersistentObject(p_database_connection),
	m_data(new AccountData)
{
}


AccountImpl::AccountImpl
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_data(new AccountData)
{
	load_name_knowing_id();
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
	return m_data->name;
}

Commodity::Id
AccountImpl::commodity_id()
{
	load();
	return value(m_data->commodity_id);
}

string
AccountImpl::commodity_abbreviation()
{
	load();
	Commodity commodity(database_connection(), value(m_data->commodity_id));
	return commodity.abbreviation();
}

string
AccountImpl::description()
{
	load();
	return value(m_data->description);
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
AccountImpl::set_commodity_id(Id p_commodity_id)
{
	load();
	m_data->commodity_id = p_commodity_id;
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
	SharedSQLStatement statement
	(	*database_connection(),
		"select commodity_id, account_type_id, description "
		"from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	AccountImpl temp(*this);
	temp.m_data->commodity_id = statement.extract<Id>(0);
	temp.m_data->account_type =
		static_cast<AccountType>(statement.extract<int>(1));
	temp.m_data->description = statement.extract<string>(2);
	swap(temp);
	return;
}

void
AccountImpl::process_saving_statement(SharedSQLStatement& statement)
{
	statement.bind
	(	":account_type_id",
		static_cast<int>(value(m_data->account_type))
	);
	statement.bind(":name", m_data->name);
	statement.bind(":description", value(m_data->description));
	statement.bind(":commodity_id", value(m_data->commodity_id));
	statement.step_final();
	return;
}

void
AccountImpl::do_save_existing()
{
	SharedSQLStatement updater
	(	*database_connection(),
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
	SharedSQLStatement inserter
	(	*database_connection(),
		"insert into accounts(account_type_id, name, description, "
		"commodity_id) values(:account_type_id, :name, :description, "
		":commodity_id)"
	);
	process_saving_statement(inserter);
	return;
}

string
AccountImpl::primary_table_name()
{
	return "accounts";
}

void
AccountImpl::load_name_knowing_id()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select name from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	m_data->name = statement.extract<string>(0);
	return;
}


void
AccountImpl::load_id_knowing_name()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select account_id from accounts where name = :p"
	);
	statement.bind(":p", m_data->name);
	statement.step();
	set_id(statement.extract<Id>(0));
	return;
}




}  // namespace phatbooks

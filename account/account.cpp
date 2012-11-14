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
#include "commodity.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using boost::shared_ptr;
using jewel::value;
using sqloxx::DatabaseConnection;
using sqloxx::SharedSQLStatement;
using std::string;
using std::vector;

namespace phatbooks
{


vector<string>
Account::account_type_names()
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
Account::setup_tables(DatabaseConnection& dbc)
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


Account::Account
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	PersistentObject(p_database_connection),
	m_data(new AccountData)
{
}


Account::Account
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_data(new AccountData)
{
	load_name_knowing_id();
}


Account::Account
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	std::string const& p_name
):
	PersistentObject(p_database_connection),
	m_data(new AccountData)
{
	m_data->name = p_name;
	load_id_knowing_name();
}


Account::Account(Account const& rhs):
	PersistentObject(rhs),
	m_data(new AccountData(*(rhs.m_data)))
{
}


Account::~Account()
{
	/* Not necessary if m_data is a smart pointer
	delete m_data;
	m_data = 0;
	*/
}

Account::AccountType
Account::account_type()
{
	load();
	return value(m_data->account_type);
}

string
Account::name()
{
	return m_data->name;
}

Commodity::Id
Account::commodity_id()
{
	load();
	return value(m_data->commodity_id);
}

string
Account::commodity_abbreviation()
{
	load();
	Commodity commodity(database_connection(), value(m_data->commodity_id));
	return commodity.abbreviation();
}

string
Account::description()
{
	load();
	return value(m_data->description);
}

void
Account::set_account_type(AccountType p_account_type)
{
	load();
	m_data->account_type = p_account_type;
	return;
}

void
Account::set_name(string const& p_name)
{
	load();
	m_data->name = p_name;
	return;
}

void
Account::set_commodity_id(Commodity::Id p_commodity_id)
{
	load();
	m_data->commodity_id = p_commodity_id;
	return;
}

void
Account::set_description(string const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void
Account::swap(Account& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
Account::do_load()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select commodity_id, account_type_id, description "
		"from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account temp(*this);
	temp.m_data->commodity_id = statement.extract<Commodity::Id>(0);
	temp.m_data->account_type =
		static_cast<AccountType>(statement.extract<int>(1));
	temp.m_data->description = statement.extract<string>(2);
	swap(temp);
	return;
}

void
Account::process_saving_statement(SharedSQLStatement& statement)
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
Account::do_save_existing()
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
Account::do_save_new()
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
Account::primary_table_name()
{
	return "accounts";
}

void
Account::load_name_knowing_id()
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
Account::load_id_knowing_name()
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

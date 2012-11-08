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

string
Account::commodity_abbreviation()
{
	load();
	return value(m_data->commodity_abbreviation);
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
	m_data->account_type = p_account_type;
	return;
}

void
Account::set_name(string const& p_name)
{
	m_data->name = p_name;
	return;
}

void
Account::set_commodity_abbreviation(string const& p_commodity_abbreviation)
{
	m_data->commodity_abbreviation = p_commodity_abbreviation;
	return;
}

void
Account::set_description(string const& p_description)
{
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
Account::do_load_all()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select commodity_id, account_type_id, description "
		"from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account temp(*this);
	Commodity commodity
	(	database_connection(),
		statement.extract<Commodity::Id>(0)
	);
	temp.set_account_type
	(	static_cast<AccountType>(statement.extract<int>(1))
	);
	temp.set_description(statement.extract<string>(2));
	temp.set_commodity_abbreviation(commodity.abbreviation());
	swap(temp);
	return;
}


void
Account::do_save_new_all()
{
	Commodity commodity(database_connection(), commodity_abbreviation());
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into accounts(account_type_id, name, description, "
		"commodity_id) values(:account_type_id, :name, :description, "
		":commodity_id)"
	);
	statement.bind
	(	":account_type_id", static_cast<int>(value(m_data->account_type))
	);
	statement.bind(":name", m_data->name);
	statement.bind(":description", value(m_data->description));
	statement.bind(":commodity_id", commodity.id());
	statement.step_final();
	return;
}


string
Account::do_get_table_name() const
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
	set_name(statement.extract<string>(0));
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

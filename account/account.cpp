#include "account.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sql_statement.hpp"
#include <cassert>
#include <string>
#include <vector>

/** \file account.cpp
 *
 * \brief Source file for code pertaining to Account class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


using sqloxx::DatabaseConnection;
using sqloxx::SQLStatement;
using std::string;
using std::vector;

namespace phatbooks
{

Account::Account
(	string p_name,
	string p_commodity_abbreviation,
	AccountType p_account_type,
	string p_description
):
	m_name(p_name),
	m_commodity_abbreviation(p_commodity_abbreviation),
	m_account_type(p_account_type),
	m_description(p_description)
{
}

vector<string>
Account::account_type_names()
{
	static bool calculated_already = false;
	static vector<string> ret;
	while (!calculated_already)
	{
		ret.push_back("Asset");
		ret.push_back("Liability");
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
		"create view accounts_extended as "
		"select account_id, account_type_id, accounts.name, "
		"accounts.description, commodities.abbreviation, "
		"commodity_id, "
		"commodities.multiplier_to_base_intval, "
		"commodities.multiplier_to_base_places, "
		"commodities.precision from "
		"accounts join commodities using(commodity_id);"
	);
	return;
}

Account::AccountType
Account::account_type()
{
	load();
	return *m_account_type;
}

string Account::name()
{
	load();
	return *m_name;
}

string Account::commodity_abbreviation()
{
	load();
	return *m_commodity_abbreviation;
}

string Account::description()
{
	load();
	return *m_description;
}

void
Account::set_account_type(AccountType p_account_type)
{
	m_account_type = p_account_type;
	return;
}

void
Account::set_name(string const& p_name)
{
	m_name = p_name;
	return;
}

void
Account::set_commodity_abbreviation(string const& p_commodity_abbreviation)
{
	m_commodity_abbreviation = p_commodity_abbreviation;
	return;
}

void
Account::set_description(string const& p_description)
{
	m_description = p_description;
	return;
}

void
Account::do_load_all()
{
	SQLStatement statement
	(	*database_connection(),
		"select name, abbreviation, account_type_id, description from "
		"accounts_extended where name = :p"
	);
	statement.bind(":p", name);
	statement.step();
	string const n = statement.extract<string>(0);
	string const comm_abb = statement.extract<string>(1);
	int const atid =
		static_cast<Account::AccountType>(statement.extract<int>(2));
	string const d = statement.extract<string>(3);
	set_name(n);
	set_commodity_abbreviation(comm_abb);
	set_account_type(atid);
	set_description(d);
	return;
}

/*
 * These need implementing
void
Account::do_save_existing_all()
{
}

void
Account::do_save_existing_partial()
{
}
*/

void
Account::do_save_new_all()
{
	Commodity comm(database_connection(), *m_commodity_abbreviation);
	SQLStatement statement
	(	*database_connection(),
		"insert into accounts(account_type_id, name, description, "
		"commodity_id) values(:account_type_id, :name, :description, "
		":commodity_id)"
	);
	statement.bind(":account_type_id", static_cast<int>(*m_account_type));
	statement.bind(":description", *m_description);
	statement.bind(":commodity_id", comm.id());
	statement.quick_step();
	return;
}

std::string
do_get_table_name()
{
	return "accounts";
}

	

}  // namespace phatbooks

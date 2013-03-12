/** \file account_impl.cpp
 *
 * \brief Source file for code pertaining to AccountImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "account_impl.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "b_string.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using boost::numeric_cast;
using boost::shared_ptr;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using sqloxx::SQLStatement;
using std::string;
using std::vector;

#ifdef DEBUG
	#include <iomanip>
	#include <iostream>
#endif




namespace phatbooks
{


typedef
	PhatbooksDatabaseConnection::BalanceCacheAttorney
	BalanceCacheAttorney;



void
AccountImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table account_types(account_type_id integer primary key)"
	);
	vector<BString>::size_type const num_account_types =
		account_type_names().size();
	for (vector<BString>::size_type i = 1; i <= num_account_types; ++i)
	{
		SQLStatement statement
		(	dbc,	
			"insert into account_types(account_type_id) values(:p)"
		);
		statement.bind(":p", numeric_cast<Id>(i));
		statement.step_final();
	}

#	ifndef DEBUG
		SQLStatement checker
		(	dbc,
			"select max(account_type_id) from account_types"
		);
		checker.step();
		Id const maxi = checker.extract<Id>(0);
		assert (maxi == 6);
		checker.step_final();
#	endif

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

	return;
}


AccountImpl::Id
AccountImpl::id_for_name
(	PhatbooksDatabaseConnection& dbc,
	BString const& name
)
{
	SQLStatement statement
	(	dbc,
		"select account_id from accounts where name = :name"
	);
	statement.bind(":name", bstring_to_std8(name));
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
}

bool
AccountImpl::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	BString const& p_name
)
{
	SQLStatement statement
	(	p_database_connection,
		"select name from accounts where name = :p"
	);
	statement.bind(":p", bstring_to_std8(p_name));
	return statement.step();
}

bool
AccountImpl::none_saved_with_account_type
(	PhatbooksDatabaseConnection& p_database_connection,
	account_type::AccountType p_account_type
)
{
	SQLStatement statement
	(	p_database_connection,
		"select account_type_id from accounts where "
		"account_type_id = :p"
	);
	statement.bind(":p", static_cast<int>(p_account_type));
	return !statement.step();
}

bool
AccountImpl::none_saved_with_account_super_type
(	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type
)
{
	SQLStatement statement
	(	p_database_connection,
		"select account_type_id from accounts"
	);
	while (statement.step())
	{
		AccountType const atype =
			static_cast<AccountType>(statement.extract<int>(0));
		if (super_type(atype) == p_account_super_type)
		{
			return false;
		}
	}
	return true;
}


AccountImpl::AccountType
AccountImpl::account_type()
{
	load();
	return value(m_data->account_type);
}

AccountImpl::AccountSuperType
AccountImpl::account_super_type()
{
	load();
	return super_type(account_type());
}

BString
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

BString
AccountImpl::description()
{
	load();
	return value(m_data->description);
}

Decimal
AccountImpl::technical_balance()
{
	load();  // This may be unnecessary but there's no harm in it.
	return BalanceCacheAttorney::technical_balance
	(	database_connection(),
		id()
	);
}

Decimal
AccountImpl::friendly_balance()
{
	load();
	Decimal const tecbal = technical_balance();
	switch (value(m_data->account_type))
	{
	// todo Should equity have sign switched? Will we ever
	// display equity accounts anyway?
	// todo Deal with remote possibility of exception
	// on sign change? Or is this
	// ruled out by higher level code?

	case account_type::asset:  // Fall through
	case account_type::liability:  // Fall through
	case account_type::equity:
		return tecbal;
		assert (false);  // Execution never reaches here.

	case account_type::revenue:  // Fall through
	case account_type::expense:  // Fall through
	case account_type::pure_envelope:
		return round(tecbal * Decimal(-1, 0), tecbal.places());
		assert (false);  // Execution never reaches here.

	default:
		assert (false);  // Execution never reaches here.
	}
}
	
	

void
AccountImpl::set_account_type(AccountType p_account_type)
{
	load();
	m_data->account_type = p_account_type;
	return;
}

void
AccountImpl::set_name(BString const& p_name)
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
AccountImpl::set_description(BString const& p_description)
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
	temp.m_data->name = std8_to_bstring(statement.extract<string>(0));
	temp.m_data->commodity = Commodity
	(	database_connection(),
		statement.extract<Id>(1)
	);
	temp.m_data->account_type =
		static_cast<AccountType>(statement.extract<int>(2));
	temp.m_data->description = std8_to_bstring(statement.extract<string>(3));
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
	statement.bind(":name", bstring_to_std8(value(m_data->name)));
	statement.bind
	(	":description",
		bstring_to_std8(value(m_data->description))
	);
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
	string const statement_text =
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
	// non-throwing).
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
AccountImpl::exclusive_table_name()
{
	return primary_table_name();
}

string
AccountImpl::primary_key_name()
{
	return "account_id";
}




}  // namespace phatbooks

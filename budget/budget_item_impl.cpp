// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "budget_item_impl.hpp"
#include "account.hpp"
#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_conv.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/exception.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/string.h>
#include <string>

using jewel::clear;
using jewel::Decimal;
using jewel::UninitializedOptionalException;
using jewel::value;
using sqloxx::IdentityMap;
using sqloxx::SQLStatement;
using std::string;


// For debugging
#include <jewel/log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff

namespace phatbooks
{


static_assert
(	boost::is_same<Account::Id, BudgetItem::Id>::value,
	"Account::Id needs to be the same type as BudgetItem::Id."
);


typedef
	PhatbooksDatabaseConnection::BudgetAttorney
	BudgetAttorney;

	
void
BudgetItemImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table budget_items"
		"("
			"budget_item_id integer primary key autoincrement, "
			// TODO Should account_id be unique here?
			"account_id not null references accounts, "
			"description text, "
			"interval_units integer not null, "
			"interval_type_id integer not null references interval_types, "
			"amount integer not null"
		");"
	);
	return;
}
	

BudgetItemImpl::BudgetItemImpl
(	IdentityMap& p_identity_map
):
	PersistentObject(p_identity_map),
	m_data(new BudgetItemData)
{
}

BudgetItemImpl::BudgetItemImpl
(	IdentityMap& p_identity_map,
	Id p_id
):
	PersistentObject(p_identity_map, p_id),
	m_data(new BudgetItemData)
{
}


BudgetItemImpl::~BudgetItemImpl()
{
}


std::string
BudgetItemImpl::primary_table_name()
{
	return "budget_items";
}

std::string
BudgetItemImpl::exclusive_table_name()
{
	return primary_table_name();
}

std::string
BudgetItemImpl::primary_key_name()
{
	return "budget_item_id";	
}

void
BudgetItemImpl::mimic(BudgetItemImpl& rhs)
{
	load();
	BudgetItemImpl temp(*this);
	temp.set_description(rhs.description());
	temp.set_account(rhs.account());
	temp.set_frequency(rhs.frequency());
	temp.set_amount(rhs.amount());
	swap(temp);
	return;
}

BudgetItemImpl::BudgetItemImpl(BudgetItemImpl const& rhs):
	PersistentObject(rhs),
	m_data(new BudgetItemData(*(rhs.m_data)))
{
}

void
BudgetItemImpl::set_description(wxString const& p_description)
{
	load();
	m_data->set_description(p_description);
	return;
}

void
BudgetItemImpl::set_account(Account const& p_account)
{
	load();
	m_data->set_account(p_account);
	return;
}

void
BudgetItemImpl::set_frequency(Frequency const& p_frequency)
{
	load();
	m_data->set_frequency(p_frequency);
	return;
}

void
BudgetItemImpl::set_amount(Decimal const& p_amount)
{
	load();
	m_data->set_amount(p_amount);
	return;
}

wxString
BudgetItemImpl::description()
{
	load();
	return m_data->description();
}

Account
BudgetItemImpl::account()
{
	load();
	return m_data->account();
}

Frequency
BudgetItemImpl::frequency()
{
	load();
	return m_data->frequency();
}

Decimal
BudgetItemImpl::amount()
{
	load();
	return m_data->amount();
}

void
BudgetItemImpl::swap(BudgetItemImpl& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
BudgetItemImpl::do_load()
{
	BudgetItemImpl temp(*this);
	SQLStatement statement
	(	database_connection(),
		"select account_id, description, interval_units, interval_type_id, "
		"amount from budget_items where budget_item_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account::Id const acct_id =  statement.extract<Account::Id>(0);
	Account const acct(database_connection(), acct_id);
	Decimal const amt
	(	statement.extract<Decimal::int_type>(4),
		acct.commodity().precision()
	);
	temp.m_data->set_account(Account(database_connection(), acct_id));
	temp.m_data->
		set_description(std8_to_wx(statement.extract<string>(1)));
	using interval_type::IntervalType;
	temp.m_data->set_frequency
	(	Frequency
		(	statement.extract<int>(2),
			static_cast<IntervalType>(statement.extract<int>(3))
		)
	);
	temp.m_data->set_amount(amt);
	swap(temp);
}

void
BudgetItemImpl::process_saving_statement(SQLStatement& statement)
{
	JEWEL_ASSERT (m_data->account().has_id());
	statement.bind(":account_id", m_data->account().id());
	statement.bind
	(	":description",
		wx_to_std8(m_data->description())
	);
	Frequency const freq = m_data->frequency();
	statement.bind(":interval_units", freq.num_steps());
	statement.bind(":interval_type_id", freq.step_type());
	statement.bind(":amount", m_data->amount().intval());
	statement.step_final();
	return;
}

void
BudgetItemImpl::do_save_existing()
{
	SQLStatement updater
	(	database_connection(),
		"update budget_items set "
		"account_id = :account_id, "
		"description = :description, "
		"interval_units = :interval_units, "
		"interval_type_id = :interval_type_id, "
		"amount = :amount "
		"where budget_item_id = :budget_item_id"
	);
	updater.bind(":budget_item_id", id());
	process_saving_statement(updater);
	BudgetAttorney::regenerate(database_connection());
	return;
}

void
BudgetItemImpl::do_save_new()
{
	SQLStatement inserter
	(	database_connection(),
		"insert into budget_items"
		"("
			"account_id, "
			"description, "
			"interval_units, "
			"interval_type_id, "
			"amount"
		") "
		"values"
		"("
			":account_id, "
			":description, "
			":interval_units, "
			":interval_type_id, "
			":amount"
		")"
	);
	process_saving_statement(inserter);
	BudgetAttorney::regenerate(database_connection());
	return;
}
			
void
BudgetItemImpl::do_ghostify()
{
	m_data->clear();
	return;
}

void
BudgetItemImpl::do_remove()
{
	string const statement_text =
		"delete from " + primary_table_name() + " where " +
		primary_key_name() + " = :p";
	SQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());
	statement.step_final();
	BudgetAttorney::regenerate(database_connection());
	return;
}

BudgetItemImpl::BudgetItemData::BudgetItemData():
	m_account(0)
{
}

BudgetItemImpl::BudgetItemData::~BudgetItemData()
{
	delete m_account;
	m_account = 0;
}

BudgetItemImpl::BudgetItemData::BudgetItemData(BudgetItemData const& rhs):
	m_account(0),
	m_description(rhs.m_description),
	m_frequency(rhs.m_frequency),
	m_amount(rhs.m_amount)
{
	if (rhs.m_account)
	{
		m_account = new Account(rhs.account());
	}
}

Account
BudgetItemImpl::BudgetItemData::account() const
{
	if (m_account)
	{
		return *m_account;
	}
	JEWEL_ASSERT (!m_account);
	JEWEL_THROW
	(	UninitializedOptionalException,
		"BudgetItemImpl::BudgetItemData::m_account is null."
	);
}

wxString
BudgetItemImpl::BudgetItemData::description() const
{
	return value(m_description);
}

Frequency
BudgetItemImpl::BudgetItemData::frequency() const
{
	return value(m_frequency);
}

Decimal
BudgetItemImpl::BudgetItemData::amount() const
{
	return value(m_amount);
}

void
BudgetItemImpl::BudgetItemData::set_account(Account const& p_account)
{
	Account* tmp = new Account(p_account);
	if (m_account)
	{
		delete m_account;
		m_account = 0;
	}
	JEWEL_ASSERT (!m_account);
	m_account = tmp;
	return;
}

void
BudgetItemImpl::BudgetItemData::set_description(wxString const& p_description)
{
	m_description = p_description;
	return;
}

void
BudgetItemImpl::BudgetItemData::set_frequency(Frequency const& p_frequency)
{
	m_frequency = p_frequency;
	return;
}

void
BudgetItemImpl::BudgetItemData::set_amount(Decimal const& p_amount)
{
	m_amount = p_amount;
	return;
}

void
BudgetItemImpl::BudgetItemData::clear()
{
	using jewel::clear;
	if (m_account)
	{
		delete m_account;
		m_account = 0;
	}
	clear(m_description);
	clear(m_frequency);
	clear(m_amount);
}

}  // namespace phatbooks


/** \file entry.cpp
 *
 * \brief Source file pertaining to EntryImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "entry.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/handle.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <string>

using sqloxx::SharedSQLStatement;
using boost::shared_ptr;
using jewel::Decimal;
using jewel::value;
using std::string;

namespace phatbooks
{

void EntryImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table entries"
		"("
			"entry_id integer primary key autoincrement, "
			"journal_id not null references journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null"
		");"
	);
	return;
}

EntryImpl::EntryImpl
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection
):
	PersistentObject(p_database_connection),
	m_data(new EntryData)
{
}


EntryImpl::EntryImpl
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_data(new EntryData)
{
}


EntryImpl::~EntryImpl()
{
	/* If m_data is a smart pointer, this is not required.
	delete m_data;
	m_data = 0;
	*/
}


void
EntryImpl::set_journal_id(Journal::Id p_journal_id)
{
	load();
	m_data->journal_id = p_journal_id;
	return;
}


void
EntryImpl::set_account(Account const& p_account)
{
	load();
	m_data->account = p_account;
	return;
}


void
EntryImpl::set_comment(string const& p_comment)
{
	load();
	m_data->comment = p_comment;
	return;
}


void
EntryImpl::set_amount(Decimal const& p_amount)
{
	load();
	m_data->amount = p_amount;
	return;
}

Account
EntryImpl::account()
{
	load();
	return value(m_data->account);
}

std::string
EntryImpl::account_name()
{
	load();
	return value(m_data->account).name();
}


string
EntryImpl::comment()
{
	load();
	return value(m_data->comment);
}


jewel::Decimal
EntryImpl::amount()
{
	load();
	return value(m_data->amount);
}


void
EntryImpl::swap(EntryImpl& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}


EntryImpl::EntryImpl(EntryImpl const& rhs):
	PersistentObject(rhs),
	m_data(new EntryData(*(rhs.m_data)))
{
}
	

void
EntryImpl::do_load()
{
	EntryImpl temp(*this);
	SharedSQLStatement statement
	(	*database_connection(),
		"select account_id, comment, amount, journal_id from entries where "
		"entry_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account const acct
	(	database_connection(),
		statement.extract<Account::Id>(0)
	);
	Decimal const amt
	(	statement.extract<boost::int64_t>(2),
		acct.commodity().precision()
	);

	temp.m_data->account = acct;
	temp.m_data->comment = statement.extract<string>(1);
	temp.m_data->amount = amt;
	temp.m_data->journal_id = statement.extract<Journal::Id>(3);
	
	swap(temp);
	return;
}


void
EntryImpl::process_saving_statement(SharedSQLStatement& statement)
{
	statement.bind(":journal_id", value(m_data->journal_id));
	statement.bind(":comment", value(m_data->comment));
	statement.bind(":account_id", value(m_data->account).id());
	statement.bind(":amount", m_data->amount->intval());
	statement.step_final();
	return;
}


void
EntryImpl::do_save_existing()
{
	SharedSQLStatement updater
	(	*database_connection(),
		"update entries set "
		"journal_id = :journal_id, "
		"comment = :comment, "
		"account_id = :account_id, "
		"amount = :amount "
		"where entry_id = :entry_id"
	);
	updater.bind(":entry_id", id());
	process_saving_statement(updater);
	return;
}


void
EntryImpl::do_save_new()
{
	SharedSQLStatement inserter
	(	*database_connection(),
		"insert into entries(journal_id, comment, account_id, amount) "
		"values(:journal_id, :comment, :account_id, :amount)"
	);
	process_saving_statement(inserter);
	return;
}


std::string
EntryImpl::primary_table_name()
{
	return "entries";
}



}  // namespace phatbooks

/** \file entry.cpp
 *
 * \brief Source file pertaining to Entry class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "entry.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

using sqloxx::DatabaseConnection;
using sqloxx::SharedSQLStatement;
using boost::shared_ptr;
using jewel::Decimal;
using std::string;

namespace phatbooks
{

void Entry::setup_tables(DatabaseConnection& dbc)
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


Entry::Entry(shared_ptr<DatabaseConnection> p_database_connection):
	PersistentObject(p_database_connection)
{
}


Entry::Entry(shared_ptr<DatabaseConnection> p_database_connection, Id p_id):
	PersistentObject(p_database_connection, p_id)
{
}


void
Entry::set_journal_id(Journal::Id p_journal_id)
{
	m_journal_id = p_journal_id;
	return;
}


void
Entry::set_account_name(string const& p_account_name)
{
	m_account_name = p_account_name;
	return;
}


void
Entry::set_comment(string const& p_comment)
{
	m_comment = p_comment;
	return;
}


void
Entry::set_amount(Decimal const& p_amount)
{
	m_amount = p_amount;
	return;
}


std::string
Entry::account_name()
{
	load();
	return *m_account_name;
}


string
Entry::comment()
{
	load();
	return *m_comment;
}


jewel::Decimal
Entry::amount()
{
	load();
	return *m_amount;
}


void
Entry::do_load_all()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select account_id, comment, amount, journal_id from entries where "
		"entry_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account acct(database_connection(), statement.extract<Account::Id>(0));
	string const acct_name = acct.name();
	string const cmt = statement.extract<string>(1);
	Commodity cmd(database_connection(), acct.commodity_abbreviation());
	Decimal const amt(statement.extract<boost::int64_t>(2), cmd.precision());
	Journal::Id const jid = statement.extract<Journal::Id>(3);
	set_account_name(acct_name);
	set_comment(cmt);
	set_amount(amt);
	set_journal_id(jid);
	return;
}


void
Entry::do_save_new_all()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into entries(journal_id, comment, account_id, amount) "
		"values(:journal_id, :comment, :account_id, :amount)"
	);
	Account account(database_connection(), account_name());
	statement.bind(":journal_id", *m_journal_id);
	statement.bind(":comment", comment());
	statement.bind(":account_id", account.id());
	statement.bind(":amount", amount().intval());
	statement.step_final();
	return;
}


std::string
Entry::do_get_table_name()
{
	return "entries";
}



}  // namespace phatbooks

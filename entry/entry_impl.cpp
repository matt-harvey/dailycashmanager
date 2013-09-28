// Copyright (c) 2013, Matthew Harvey. All rights reserved.


/** \file entry_impl.cpp
 *
 * \brief Source file pertaining to EntryImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "entry_impl.hpp"
#include "account.hpp"
#include "date.hpp"
#include "string_conv.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_conv.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <sqloxx/database_connection.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/string.h>
#include <memory>
#include <string>
#include <utility>

using sqloxx::SQLStatement;
using boost::optional;
using boost::shared_ptr;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using std::move;
using std::string;
using std::unique_ptr;

namespace gregorian = boost::gregorian;

namespace phatbooks
{

void EntryImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table transaction_sides"
		"("
			"transaction_side_id integer primary key"
		");"
	);
	using transaction_side::TransactionSide;
	for
	(	int i = 0;
		i != static_cast<int>(transaction_side::num_transaction_sides);
		++i
	)
	{
		SQLStatement statement
		(	dbc,
			"insert into transaction_sides(transaction_side_id) values(:p)"
		);
		statement.bind(":p", i);
		statement.step_final();
	}
	dbc.execute_sql
	(	"create table entries"
		"("
			"entry_id integer primary key autoincrement, "
			"journal_id not null references journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null, "
			"is_reconciled not null references booleans, "
			"transaction_side_id not null references transaction_sides"
		");"
	);
	dbc.execute_sql
	(	"create index entry_journal_index on entries(journal_id); "
	);
	return;
}

EntryImpl::EntryImpl
(	IdentityMap& p_identity_map
):
	PersistentObject(p_identity_map),
	m_data(new EntryData)
{
}


EntryImpl::EntryImpl
(	IdentityMap& p_identity_map,
	Id p_id
):
	PersistentObject(p_identity_map, p_id),
	m_data(new EntryData)
{
}

void
EntryImpl::set_journal_id(Id p_journal_id)
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
EntryImpl::set_comment(wxString const& p_comment)
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

void
EntryImpl::set_whether_reconciled(bool p_is_reconciled)
{
	load();
	m_data->is_reconciled = p_is_reconciled;
	return;
}

void
EntryImpl::set_transaction_side
(	transaction_side::TransactionSide p_transaction_side
)
{
	load();
	m_data->transaction_side = p_transaction_side;
}

Account
EntryImpl::account()
{
	load();
	return value(m_data->account);
}

wxString
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

bool
EntryImpl::is_reconciled()
{
	load();
	return value(m_data->is_reconciled);
}

transaction_side::TransactionSide
EntryImpl::transaction_side()
{
	load();
	return value(m_data->transaction_side);
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

EntryImpl::EntryImpl(EntryImpl&& rhs):
	PersistentObject(move(rhs)),
	m_data(move(rhs.m_data))
{
	rhs.m_data = nullptr;
}

void
EntryImpl::do_load()
{
	EntryImpl temp(*this);
	SQLStatement statement
	(	database_connection(),
		"select account_id, comment, amount, journal_id, is_reconciled, "
		"transaction_side_id "
		" from entries where "
		"entry_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account const acct
	(	database_connection(),
		statement.extract<Account::Id>(0)
	);
	Decimal const amt
	(	statement.extract<Decimal::int_type>(2),
		acct.commodity().precision()
	);

	temp.m_data->account = acct;
	temp.m_data->comment = std8_to_wx(statement.extract<string>(1));
	temp.m_data->amount = amt;
	temp.m_data->journal_id = statement.extract<Id>(3);
	temp.m_data->is_reconciled =
		static_cast<bool>(statement.extract<int>(4));
	temp.m_data->transaction_side =
		static_cast<transaction_side::TransactionSide>
		(	statement.extract<int>(5)
		);
	
	swap(temp);
	return;
}


void
EntryImpl::process_saving_statement(SQLStatement& statement)
{
	statement.bind(":journal_id", value(m_data->journal_id));
	statement.bind(":comment", wx_to_std8(value(m_data->comment)));
	statement.bind(":account_id", value(m_data->account).id());
	statement.bind(":amount", m_data->amount->intval());
	statement.bind
	(	":is_reconciled",
		static_cast<int>(value(m_data->is_reconciled))
	);
	statement.bind(":transaction_side_id", value(m_data->transaction_side));
	statement.step_final();
	return;
}


void
EntryImpl::do_save_existing()
{
	// We need to get the old Account so we can mark it as stale
	SQLStatement old_account_capturer
	(	database_connection(),
		"select account_id from entries where entry_id = :p"
	);
	old_account_capturer.bind(":p", id());
	old_account_capturer.step();
	PhatbooksDatabaseConnection::BalanceCacheAttorney::mark_as_stale
	(	database_connection(),
		old_account_capturer.extract<Account::Id>(0)
	);
	old_account_capturer.step_final();

	// And we also need to mark the new Account as stale
	PhatbooksDatabaseConnection::BalanceCacheAttorney::mark_as_stale
	(	database_connection(),
		account().id()
	);

	// And now we can update the Entry itself
	SQLStatement updater
	(	database_connection(),
		"update entries set "
		"journal_id = :journal_id, "
		"comment = :comment, "
		"account_id = :account_id, "
		"amount = :amount, "
		"is_reconciled = :is_reconciled, "
		"transaction_side_id = :transaction_side_id "
		"where entry_id = :entry_id"
	);
	updater.bind(":entry_id", id());
	process_saving_statement(updater);
	return;
}


void
EntryImpl::do_save_new()
{
	PhatbooksDatabaseConnection::BalanceCacheAttorney::mark_as_stale
	(	database_connection(),
		account().id()
	);
	SQLStatement inserter
	(	database_connection(),
		"insert into entries"
		"("
			"journal_id, "
			"comment, "
			"account_id, "
			"amount, "
			"is_reconciled, "
			"transaction_side_id"
		") "
		"values"
		"("
			":journal_id, "
			":comment, "
			":account_id, "
			":amount, "
			":is_reconciled, "
			":transaction_side_id"
		")"
	);
	process_saving_statement(inserter);
	return;
}

void
EntryImpl::do_ghostify()
{
	clear(m_data->journal_id);
	clear(m_data->account);
	clear(m_data->comment);
	clear(m_data->amount);
	clear(m_data->is_reconciled);
	clear(m_data->transaction_side);
	return;
}

void
EntryImpl::do_remove()
{
	PhatbooksDatabaseConnection::BalanceCacheAttorney::mark_as_stale
	(	database_connection(),
		account().id()
	);
	std::string const statement_text =
		"delete from " + primary_table_name() + " where " +
		primary_key_name() + " = :p";
	SQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());
	statement.step_final();
}




std::string
EntryImpl::primary_table_name()
{
	return "entries";
}

std::string
EntryImpl::exclusive_table_name()
{
	return primary_table_name();
}

std::string
EntryImpl::primary_key_name()
{
	return "entry_id";
}

void
EntryImpl::mimic(EntryImpl& rhs)
{
	load();
	EntryImpl temp(*this);
	temp.set_account(rhs.account());
	temp.set_comment(rhs.comment());
	temp.set_amount(rhs.amount());
	temp.set_whether_reconciled(rhs.is_reconciled());
	temp.set_transaction_side(rhs.transaction_side());
	swap(temp);
	return;
}
	
unique_ptr<SQLStatement>
create_date_ordered_actual_ordinary_entry_selector_aux
(	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date,
	optional<Account> const& p_maybe_account
)
{
	// TODO Factor out duplicated code between here and
	// "entry_table_iterator.cpp".
#	ifndef NDEBUG
		// Ensure we are picking all and only the
		// actual transactions.
		int const target_non_actual_type = 3;
		int i = 0;
		int const lim =
			static_cast<int>(transaction_type::num_transaction_types);
		for ( ; i != lim; ++i)
		{
			transaction_type::TransactionType const ttype =
				static_cast<transaction_type::TransactionType>(i);
			if (ttype == target_non_actual_type)
			{
				JEWEL_ASSERT (!transaction_type_is_actual(ttype));
			}
			else
			{
				JEWEL_ASSERT (transaction_type_is_actual(ttype));
			}
		}
#	endif

	string text =
		"select entry_id from entries join ordinary_journal_detail "
		"using(journal_id) join journals using(journal_id) where "
		"transaction_type_id != 3";
	if (p_maybe_min_date) text += " and date >= :min_date";
	if (p_maybe_max_date) text += " and date <= :max_date";
	if (p_maybe_account) text += " and account_id = :account_id";
	unique_ptr<SQLStatement> ret
	(	new SQLStatement(p_database_connection, text)
	);
	if (p_maybe_min_date)
	{
		ret->bind(":min_date", julian_int(*p_maybe_min_date));
	}
	if (p_maybe_max_date)
	{
		ret->bind(":max_date", julian_int(*p_maybe_max_date));
	}
	if (p_maybe_account && p_maybe_account->has_id())
	{
		ret->bind(":account_id", p_maybe_account->id());
	}
	return move(ret);
}


}  // namespace phatbooks

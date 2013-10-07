// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "ordinary_journal.hpp"
#include "commodity.hpp"
#include "draft_journal_handle.hpp"
#include "date.hpp"
#include "entry_handle.hpp"
#include "ordinary_journal.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/string.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

using boost::numeric_cast;
using boost::optional;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::endl;
using std::is_same;
using std::string;
using std::vector;


namespace gregorian = boost::gregorian;


namespace phatbooks
{

string
OrdinaryJournal::exclusive_table_name()
{
	return "ordinary_journal_detail";
}

void
OrdinaryJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table ordinary_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"date integer not null"
		"); "
	);
	dbc.execute_sql
	(	"create index journal_date_index on ordinary_journal_detail(date); "
	);
	return;
}

OrdinaryJournal::OrdinaryJournal
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentJournal(p_identity_map, p_signature)
{
}

OrdinaryJournal::OrdinaryJournal
(	IdentityMap& p_identity_map,
	Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentJournal(p_identity_map, p_id, p_signature)
{
}
		
OrdinaryJournal::OrdinaryJournal(OrdinaryJournal const& rhs):
	PersistentJournal(rhs),
	m_date(rhs.m_date)
{
}

void
OrdinaryJournal::set_date(gregorian::date const& p_date)
{
	if (p_date < database_connection().entity_creation_date())
	{
		JEWEL_THROW
		(	InvalidJournalDateException,
			"Date of OrdinaryJournal cannot be set to a date "
			"earlier than the entity creation date, using the "
			"set_date function."
		);
	}
	JEWEL_ASSERT (p_date != database_connection().opening_balance_journal_date());
	load();
	set_date_unrestricted(p_date);
	JEWEL_ASSERT
	(	boost_date_from_julian_int(value(m_date)) >=
		database_connection().entity_creation_date()
	);
	return;
}

void
OrdinaryJournal::set_date_unrestricted(gregorian::date const& p_date)
{
	load();
	m_date = julian_int(p_date);
	return;
}

gregorian::date
OrdinaryJournal::date()
{
	load();
	return boost_date_from_julian_int(value(m_date));
}

void
OrdinaryJournal::swap(OrdinaryJournal& rhs)
{
	PersistentJournal::swap(rhs);
	using std::swap;
	swap(m_date, rhs.m_date);
	return;
}

void
OrdinaryJournal::do_load()
{
	OrdinaryJournal temp(*this);

	// Load the Journal (base) part of temp.
	temp.load_journal_core();

	// Load the derived, OrdinaryJournal part of temp.
	SQLStatement statement
	(	database_connection(),
		"select date from ordinary_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	// If this assertion ever fails, it's a reminder that the exception-safety
	// of loading here MAY depend on m_date being of a native, non-throwing
	// type.
	static_assert
	(	is_same<DateRep, int>::value,
		"DateRep needs to be int."
	);
	temp.m_date = numeric_cast<DateRep>(statement.extract<long long>(0));
	swap(temp);
	return;
}

void
OrdinaryJournal::do_save_new()
{
	// Save the Journal	(base) part of the object and record the id.
	Id const journal_id = save_new_journal_core();

	// Save the derived, OrdinaryJournal part of the object
	SQLStatement statement
	(	database_connection(),
		"insert into ordinary_journal_detail (journal_id, date) "
		"values(:journal_id, :date)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":date", value(m_date));
	statement.step_final();

	return;
}

void
OrdinaryJournal::do_save_existing()
{
	JEWEL_LOG_TRACE();

	// Save the Journal (base) part of the object
	save_existing_journal_core();
	JEWEL_LOG_TRACE();

	// Save the derived, OrdinaryJournal part of the object
	SQLStatement updater
	(	database_connection(),	
		"update ordinary_journal_detail set date = :date "
		"where journal_id = :journal_id"
	);
	updater.bind(":date", value(m_date));
	updater.bind(":journal_id", id());
	updater.step_final();
	JEWEL_LOG_TRACE();
	return;
}

void
OrdinaryJournal::do_ghostify()
{
	ghostify_journal_core();
	clear(m_date);
	return;
}

void
OrdinaryJournal::do_remove()
{
	// Note this is wrapped in PersistentObject::remove, which
	// (a) wraps it in a DatabaseTransaction, and
	// (b) calls ghostify() if an exception is thrown.
	// This makes it exception-safe as a whole.
	SQLStatement journal_detail_deleter
	(	database_connection(),
		"delete from ordinary_journal_detail where "
		"journal_id = :p"
	);
	journal_detail_deleter.bind(":p", id());
	SQLStatement journal_master_deleter
	(	database_connection(),
		"delete from journals where journal_id = :p"
	);
	journal_master_deleter.bind(":p", id());
	clear_entries();
	save();
	journal_detail_deleter.step_final();
	journal_master_deleter.step_final();
	return;
}

void
OrdinaryJournal::mimic(Journal& rhs)
{
	load();
	OrdinaryJournal temp(*this);
	optional<Id> t_id;
	if (has_id()) t_id = id();
	temp.mimic_core(rhs, database_connection(), t_id);
	swap(temp);
	return;
}

Handle<OrdinaryJournal>
create_opening_balance_journal
(	Handle<Account> const& p_account,
	Decimal const& p_desired_opening_balance
)
{
	// Normally the implementation of OrdinaryJournal function is
	// delegated to OrdinaryJournal. But here, we are using
	// a static function to return an OrdinaryJournal, which we create using
	// "high level", OrdinaryJournal-level functions; so it seems
	// appropriate to implement it here.
	PhatbooksDatabaseConnection& dbc = p_account->database_connection();
	Handle<Account> const balancing_account = dbc.balancing_account();
	Decimal const old_opening_balance =
		p_account->has_id()?
		p_account->technical_opening_balance():
		Decimal(0, p_account->commodity()->precision());
	Decimal const primary_entry_amount =
		p_desired_opening_balance - old_opening_balance;

	Handle<OrdinaryJournal> const ret(dbc);

	EntryHandle const primary_entry(dbc);
	primary_entry->set_account(p_account);
	primary_entry->set_comment("Opening balance entry");
	primary_entry->set_amount(primary_entry_amount);
	primary_entry->set_whether_reconciled(true);
	primary_entry->set_transaction_side(TransactionSide::source);
	ret->push_entry(primary_entry);

	EntryHandle const balancing_entry(dbc);
	balancing_entry->set_account(balancing_account);
	balancing_entry->set_comment("Opening balance entry");
	balancing_entry->set_amount(-primary_entry_amount);
	balancing_entry->set_whether_reconciled(false);
	balancing_entry->set_transaction_side(TransactionSide::destination);
	ret->push_entry(balancing_entry);

	ret->set_comment("Opening balance adjustment");
	if (p_account->account_super_type() == AccountSuperType::balance_sheet)
	{
		ret->set_transaction_type(TransactionType::generic);
	}
	else
	{
		ret->set_transaction_type(TransactionType::envelope);
	}

	// WARNING The source and destination Accounts are the opposite way
	// round to what would be expected by the user here; but this probably
	// doesn't matter, as the user should never see the opening balance
	// Journals directly.
	ret->set_date_unrestricted
	(	dbc.opening_balance_journal_date()
	);
	return ret;
}


}  // namespace phatbooks




/** \file journal.cpp
 *
 * \brief Source file relating to Journal class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "journal.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "entry.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include "sqloxx/sql_statement.hpp"
#include <jewel/decimal.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>

using sqloxx::DatabaseConnection;
using sqloxx::PersistentObject;
using sqloxx::SQLStatement;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::Decimal;
using std::list;
using std::string;

namespace phatbooks
{

void
Journal::setup_tables(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"is_actual integer not null references booleans, "
			"date integer not null, "
			"comment text"
		");"
	);
	dbc.execute_sql
	(	"create table entries"
		"("
			"entry_id integer primary key autoincrement, "
			"journal_id not null references journals, "
			"comment text, "
			"account_id not null references accounts, "
			"amount integer not null "
		");"
	);
	return;
}


	


Journal::Journal(shared_ptr<DatabaseConnection> p_database_connection):
	PersistentObject(p_database_connection),
	m_entries(list< shared_ptr<Entry> >()),
	m_repeaters(list< shared_ptr<Repeater> >())
{
}

Journal::Journal
(	shared_ptr<DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id)
{
}

void
Journal::set_whether_actual(bool p_is_actual)
{
	m_is_actual = p_is_actual;
	return;
}

void
Journal::set_comment(string const& p_comment)
{
	m_comment = p_comment;
	return;
}

void
Journal::set_date(DateType p_date)
{
	m_date = p_date;
	return;
}

void
Journal::add_entry(shared_ptr<Entry> entry)
{
	m_entries.push_back(entry);
	return;
}

void
Journal::add_repeater(shared_ptr<Repeater> repeater)
{
	m_repeaters.push_back(repeater);
	return;
}

bool
Journal::is_posted()
{
	if (!m_date)
	{
		return false;
	}
	else
	{
		return m_date != null_date();
	}
}

bool
Journal::is_actual()
{
	load();
	return *m_is_actual;
}

DateType
Journal::date()
{
	load();
	return *m_date;
}

string
Journal::comment()
{
	load();
	return *m_comment;
}


list< shared_ptr<Entry> > const&
Journal::entries()
{
	load();
	// WARNING Should this fail if m_entries is empty? This would
	// be the same behaviour then as the other "optionals". To be
	// truly consistent with the other optionals, it would fail
	// by means of a failed assert (assuming I haven't wrapped the
	// other optionals in some throwing construct...).
	return m_entries;
}


void
Journal::do_load_all()
{
	SQLStatement statement
	(	*database_connection(),
		"select journal_id, is_actual, date, comment from "
		"journals where journal_id = :key"
	);
	statement.bind(":key", id());
	statement.step();

	bool const is_act = static_cast<bool>(statement.extract<int>(1));
	DateType const d =
		numeric_cast<DateType>(statement.extract<boost::int64_t>(2));
	string const cmt = statement.extract<string>(3);

	SQLStatement entry_finder
	(	*database_connection(),
		"select entry_id, comment, account_id, amount from entries where "
		"journal_id = :jid"
	);
	entry_finder.bind(":jid", id());
	while (entry_finder.step())
	{
		Account acct(database_connection(), entry_finder.extract<IdType>(2));
		Commodity comm(database_connection(), acct.commodity_abbreviation());
		shared_ptr<Entry> entry
		(	new Entry
			(	acct.name(),
				entry_finder.extract<string>(1),
				Decimal
				(	entry_finder.extract<boost::int64_t>(3),
					comm.precision()
				)
			)
		);
		add_entry(entry);
	}
	set_whether_actual(is_act);
	set_date(d);
	set_comment(cmt);

	return;
}


void
Journal::do_save_new_all()
{
	IdType const journal_id = prospective_key();
	database_connection()->execute_sql("begin transaction;");
	SQLStatement statement
	(	*database_connection(),
		"insert into journals(is_actual, date, comment) "
		"values(:is_actual, :date, :comment)"
	);
	statement.bind(":is_actual", static_cast<int>(*m_is_actual));
	statement.bind(":date", (m_date? *m_date: null_date()));
	statement.bind(":comment", *m_comment);
	statement.quick_step();
	typedef list< shared_ptr<Entry> > EntryCntnr;
	typedef EntryCntnr::const_iterator Iter;
	for (Iter it = m_entries.begin(); it != m_entries.end(); ++it)
	{
		Account acct(database_connection(), (*it)->account_name());
		SQLStatement entry_storer
		(	*database_connection(),
			"insert into entries(journal_id, comment, account_id, "
			"amount) values(:journal_id, :comment, :account_id, :amount)"
		);
		entry_storer.bind(":journal_id", journal_id);
		entry_storer.bind(":comment", (*it)->comment());
		entry_storer.bind(":account_id", acct.id());
		entry_storer.bind(":amount", (*it)->amount().intval());
		entry_storer.quick_step();
	}
	database_connection()->execute_sql("end transaction;");
	return;
}


string
Journal::do_get_table_name()
{
	return "journals";
}

}  // namespace phatbooks


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
#include <boost/date_time/gregorian/gregorian.hpp>
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
	(	"create table draft_journal_detail "
		"("
			"journal_id primary key not null references journals, "
			"name text not null "
		"); "
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
Journal::set_date(boost::gregorian::date const& p_date)
{
	m_date = julian_int(p_date);
	return;
}

void
Journal::add_entry(shared_ptr<Entry> entry)
{
	if (has_id())
	{
		entry->set_journal_id(id());
	}
	m_entries.push_back(entry);
	return;
}

void
Journal::add_repeater(shared_ptr<Repeater> repeater)
{
	if (has_id())
	{
		repeater->set_journal_id(id());
	}
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
		return m_date != null_date_rep();
	}
}

bool
Journal::is_actual()
{
	load();
	return *m_is_actual;
}

boost::gregorian::date
Journal::date()
{
	load();
	return boost_date_from_julian_int(*m_date);
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
		"journals where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();

	bool const is_act = static_cast<bool>(statement.extract<int>(1));
	DateRep const d =
		numeric_cast<DateRep>(statement.extract<boost::int64_t>(2));
	string const cmt = statement.extract<string>(3);

	SQLStatement entry_finder
	(	*database_connection(),
		"select entry_id from entries where journal_id = :jid"
	);
	entry_finder.bind(":jid", id());
	while (entry_finder.step())
	{
		Entry::Id const entr_id = entry_finder.extract<Entry::Id>(0);
		shared_ptr<Entry> entry
		(	new Entry(database_connection(), entr_id)
		);
		add_entry(entry);
	}
	set_whether_actual(is_act);
	m_date = d;
	set_comment(cmt);
	
	if (!is_posted())  // Only draft journals have repeaters.
	{
		SQLStatement repeater_finder
		(	*database_connection(),
			"select repeater_id from repeaters where journal_id = :journal_id"
		);
		repeater_finder.bind(":journal_id", id());
		while (repeater_finder.step())
		{
			Repeater::Id const rep_id =
				repeater_finder.extract<Repeater::Id>(0);
			shared_ptr<Repeater> repeater
			(	new Repeater(database_connection(), rep_id)
			);
			add_repeater(repeater);
		}
	}

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
	statement.bind(":date", (m_date? *m_date: null_date_rep()));
	statement.bind(":comment", *m_comment);
	statement.quick_step();
	typedef list< shared_ptr<Entry> >::iterator EntryIter;
	for (EntryIter it = m_entries.begin(); it != m_entries.end(); ++it)
	{
		(*it)->set_journal_id(journal_id);
		(*it)->save_new();
	}
	typedef list< shared_ptr<Repeater> >::const_iterator RepIter;
	for (RepIter it = m_repeaters.begin(); it != m_repeaters.end(); ++it)
	{
		(*it)->set_journal_id(journal_id);
		(*it)->save_new();
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

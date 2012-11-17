
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
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using sqloxx::PersistentObject;
using sqloxx::SharedSQLStatement;
using boost::numeric_cast;
using boost::shared_ptr;
using boost::unordered_set;
using jewel::Decimal;
using jewel::value;
using std::logic_error;
using std::string;
using std::vector;

namespace phatbooks
{

void
Journal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.begin_transaction();
	dbc.execute_sql
	(	"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"is_actual integer not null references booleans, "
			"comment text"
		"); "
		"create table ordinary_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"date integer not null"
		"); "
		"create table draft_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"name text not null unique"
		");"
	);
	dbc.end_transaction();
	return;
}


Journal::Journal(shared_ptr<PhatbooksDatabaseConnection> p_database_connection):
	PersistentObject(p_database_connection),
	m_data(new JournalData)
{
}

Journal::Journal
(	shared_ptr<PhatbooksDatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_data(new JournalData)
{
}

Journal::~Journal()
{
	/* If m_data is a smart pointer, this is not required.
	delete m_data;
	m_data = 0;
	*/
}


void
Journal::set_whether_actual(bool p_is_actual)
{
	load();
	m_data->is_actual = p_is_actual;
	return;
}

void
Journal::set_comment(string const& p_comment)
{
	load();
	m_data->comment = p_comment;
	return;
}

void
Journal::add_entry(shared_ptr<Entry> entry)
{
	load();
	if (has_id())
	{
		entry->set_journal_id(id());
	}
	m_data->entries.push_back(entry);
	return;
}

bool
Journal::is_actual()
{
	load();
	return value(m_data->is_actual);
}

string
Journal::comment()
{
	load();
	return value(m_data->comment);
}


vector< shared_ptr<Entry> > const&
Journal::entries()
{
	load();
	// WARNING Should this fail if m_entries is empty? This would
	// be the same behaviour then as the other "optionals". To be
	// truly consistent with the other optionals, it would fail
	// by means of a failed assert (assuming I haven't wrapped the
	// other optionals in some throwing construct...).
	return m_data->entries;
}


void
Journal::swap(Journal& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
Journal::do_load()
{
	throw logic_error("Journal::do_load() should never be called!");
	return;
}


Journal::Id
Journal::do_save_new_journal_base()
{
	Id const journal_id = prospective_key();
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into journals(is_actual, comment) "
		"values(:is_actual, :comment)"
	);
	statement.bind(":is_actual", static_cast<int>(value(m_data->is_actual)));
	statement.bind(":comment", value(m_data->comment));
	statement.step_final();
	typedef vector< shared_ptr<Entry> >::iterator EntryIter;
	EntryIter const endpoint = m_data->entries.end();
	for (EntryIter it = m_data->entries.begin(); it != endpoint; ++it)
	{
		(*it)->set_journal_id(journal_id);
		(*it)->save_new();
	}
	return journal_id;
}

void
Journal::do_save_existing_journal_base()
{
	SharedSQLStatement updater
	(	*database_connection(),
		"update journals set is_actual = :is_actual, comment = :comment "
		"where journal_id = :id"
	);
	updater.bind(":is_actual", static_cast<int>(value(m_data->is_actual)));
	updater.bind(":comment", value(m_data->comment));
	updater.bind(":id", id());
	updater.step_final();
	typedef vector< shared_ptr<Entry> >::iterator EntryIter;
	EntryIter const endpoint = m_data->entries.end();
	unordered_set<Entry::Id> saved_entry_ids;
	for (EntryIter it = m_data->entries.begin(); it != endpoint; ++it)
	{
		(*it)->save();
		saved_entry_ids.insert((*it)->id());
	}
	// Remove any entries in the database with this journal's journal_id, that
	// no longer exist in the in-memory journal
	SharedSQLStatement entry_finder
	(	*database_connection(),
		"select entry_id from entries where journal_id = :journal_id"
	);
	entry_finder.bind(":journal_id", id());
	unordered_set<Entry::Id>::const_iterator const saved_entries_end =
		saved_entry_ids.end();
	while (entry_finder.step())
	{
		Entry::Id const entry_id = entry_finder.extract<Entry::Id>(0);
		if (saved_entry_ids.find(entry_id) == saved_entries_end)
		{
			// This entry is in the database but no longer in the in-memory
			// journal, so should be deleted.
			SharedSQLStatement entry_deleter
			(	*database_connection(),
				"delete from entries where entry_id = :entry_id"
			);
			entry_deleter.bind(":entry_id", entry_id);
			entry_deleter.step_final();
			// Note it's OK even if the last entry is deleted. Another
			// entry will never be reassigned its id - SQLite makes sure
			// of that - providing we let SQLite assign all the ids
			// automatically.
		}
	}
	return;
}


void
Journal::do_load_journal_base()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select is_actual, comment from journals where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Journal temp(*this);
	SharedSQLStatement entry_finder
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
		assert (has_id());
		temp.m_data->entries.push_back(entry);
	}
	temp.m_data->is_actual = static_cast<bool>(statement.extract<int>(0));
	temp.m_data->comment = statement.extract<string>(1);
	swap(temp);	
	return;
}

void
Journal::do_save_existing()
{
	throw logic_error("Journal::do_save_existing() should never be called.");
	return;
}

void
Journal::do_save_new()
{
	throw logic_error("Journal::do_save_new() should never be called!");
	return;
}
	

Journal::Journal(Journal const& rhs):
	PersistentObject(rhs),
	m_data(new JournalData(*(rhs.m_data)))
{
}


string
Journal::primary_table_name()
{
	return "journals";
}

}  // namespace phatbooks

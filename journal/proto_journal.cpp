// Copyright (c) 2013, Matthew Harvey. All rights reserved.


/** \file proto_journal.cpp
 *
 * \brief Source file relating to ProtoJournal class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "b_string.hpp"
#include "proto_journal.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "b_string.hpp"
#include "transaction_type.hpp"
#include <consolixx/table.hpp>
#include <jewel/output_aux.hpp>
#include <sqloxx/next_auto_key.hpp>
#include <sqloxx/sql_statement.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace alignment = consolixx::alignment;

using sqloxx::next_auto_key;
using sqloxx::SQLStatement;
using boost::numeric_cast;
using boost::optional;
using boost::scoped_ptr;
using boost::shared_ptr;
using boost::unordered_set;
using consolixx::Table;
using jewel::clear;
using jewel::Decimal;
using jewel::output_aux;
using jewel::value;
using std::back_inserter;
using std::ios_base;
using std::logic_error;
using std::ostream;
using std::ostringstream;
using std::remove_copy;
using std::string;
using std::vector;

#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


namespace phatbooks
{

void
ProtoJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"is_actual integer not null references booleans, "
			"comment text"
		");"
	);
	dbc.execute_sql
	(	"create table transaction_types"
		"("
			"transaction_type_id integer primary key"
		");"
	);
	using transaction_type::TransactionType;
	SQLStatement statement(dbc, "insert into transaction_types :p");
	for
	(	int i = 0;
		i != static_cast<int>(transaction_type::num_transaction_types);
		++i
	)
	{
		statement.bind(":p", i);
		statement.step_final();
	}
	return;
}


ProtoJournal::ProtoJournal():
	m_data(new ProtoJournalData)
{
}

ProtoJournal::ProtoJournal(ProtoJournal const& rhs):
	m_data(new ProtoJournalData(*(rhs.m_data)))
{
}

ProtoJournal::~ProtoJournal()
{
	/* If m_data is a smart pointer, this is not required.
	delete m_data;
	m_data = 0;
	*/
}

void
ProtoJournal::do_output(ostream& os) const
{
	Journal::output_core_journal_header(os);
	Table<Entry> table;
	push_core_journal_columns(table);
	table.populate(entries().begin(), entries().end());
	os << table;
	return;
}


vector<Entry> const& 
ProtoJournal::do_get_entries() const
{
	return m_data->entries;
}

void
ProtoJournal::do_set_whether_actual(bool p_is_actual)
{
	m_data->is_actual = p_is_actual;
	return;
}

void
ProtoJournal::do_set_comment(BString const& p_comment)
{
	m_data->comment = p_comment;
	return;
}

void
ProtoJournal::do_push_entry(Entry& entry)
{
	m_data->entries.push_back(entry);
	return;
}

void
ProtoJournal::do_remove_entry(Entry& entry)
{
	vector<Entry> temp = m_data->entries;
	m_data->entries.clear();
	remove_copy
	(	temp.begin(),
		temp.end(),
		back_inserter(m_data->entries),
		entry
	);
	return;
}
		

BString
ProtoJournal::do_get_comment() const
{
	return value(m_data->comment);
}

bool
ProtoJournal::do_get_whether_actual() const
{
	return value(m_data->is_actual);
}

void
ProtoJournal::swap(ProtoJournal& rhs)
{
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

ProtoJournal::Id
ProtoJournal::do_save_new_journal_core
(	PhatbooksDatabaseConnection& dbc
)
{
	if (!is_balanced())
	{
		throw UnbalancedJournalException
		(	"Cannot save journal core in unbalanced state."
		);
	}
	Id const journal_id = next_auto_key
	<	PhatbooksDatabaseConnection,
		Id
	>	(dbc, "journals");
	SQLStatement statement
	(	dbc,
		"insert into journals(is_actual, comment) "
		"values(:is_actual, :comment)"
	);
	statement.bind(":is_actual", static_cast<int>(value(m_data->is_actual)));
	statement.bind(":comment", bstring_to_std8(value(m_data->comment)));
	statement.step_final();
	typedef vector<Entry>::iterator EntryIter;
	EntryIter const endpoint = m_data->entries.end();
	for (EntryIter it = m_data->entries.begin(); it != endpoint; ++it)
	{
		it->set_journal_id(journal_id);
		it->save();
	}
	return journal_id;
}

void
ProtoJournal::do_save_existing_journal_core
(	PhatbooksDatabaseConnection& dbc,
	ProtoJournal::Id id
)
{
	if (!is_balanced())
	{
		throw UnbalancedJournalException
		(	"Cannot save journal core in unbalanced state."
		);
	}
	SQLStatement updater
	(	dbc,
		"update journals set is_actual = :is_actual, comment = :comment "
		"where journal_id = :id"
	);
	updater.bind(":is_actual", static_cast<int>(value(m_data->is_actual)));
	updater.bind(":comment", bstring_to_std8(value(m_data->comment)));
	updater.bind(":id", id);
	updater.step_final();
	typedef vector<Entry>::iterator EntryIter;
	EntryIter const endpoint = m_data->entries.end();
	unordered_set<Entry::Id> saved_entry_ids;
	for (EntryIter it = m_data->entries.begin(); it != endpoint; ++it)
	{
		it->save();
		saved_entry_ids.insert(it->id());
	}
	// Remove any entries in the database with this journal's journal_id, that
	// no longer exist in the in-memory journal
	SQLStatement entry_finder
	(	dbc,	
		"select entry_id from entries where journal_id = :journal_id"
	);
	entry_finder.bind(":journal_id", id);
	unordered_set<Entry::Id>::const_iterator const saved_entries_end =
		saved_entry_ids.end();
	while (entry_finder.step())
	{
		Entry::Id const entry_id = entry_finder.extract<Entry::Id>(0);
		if (saved_entry_ids.find(entry_id) == saved_entries_end)
		{
			Entry doomed_entry(dbc, entry_id);
			// This entry is in the database but no longer in the in-memory
			// journal, so should be deleted.
			doomed_entry.remove();
			// Note it's OK even if the last entry is deleted. Another
			// entry will never be reassigned its id - SQLite makes sure
			// of that - providing we let SQLite assign all the ids
			// automatically.
		}
	}
	return;
}


void
ProtoJournal::do_load_journal_core
(	PhatbooksDatabaseConnection& dbc,
	ProtoJournal::Id id
)
{
	SQLStatement statement
	(	dbc,
		"select is_actual, comment from journals where journal_id = :p"
	);
	statement.bind(":p", id);
	statement.step();
	ProtoJournal temp(*this);
	SQLStatement entry_finder
	(	dbc,	
		"select entry_id from entries where journal_id = :jid"
	);
	entry_finder.bind(":jid", id);
	while (entry_finder.step())
	{
		Entry::Id const entr_id = entry_finder.extract<Entry::Id>(0);
		Entry entry(dbc, entr_id);
		temp.m_data->entries.push_back(entry);
	}
	temp.m_data->is_actual = static_cast<bool>(statement.extract<int>(0));
	temp.m_data->comment = std8_to_bstring(statement.extract<string>(1));
	swap(temp);	
	return;
}

void
ProtoJournal::do_ghostify_journal_core()
{
	clear(m_data->is_actual);
	clear(m_data->comment);
	typedef vector<Entry>::iterator EntryIter;
	EntryIter endpoint = m_data->entries.end();
	for (EntryIter it = m_data->entries.begin(); it != endpoint; ++it)
	{
		it->ghostify();
	}
	m_data->entries.clear();
	return;
}

string
ProtoJournal::primary_table_name()
{
	return "journals";
}

string
ProtoJournal::primary_key_name()
{
	return "journal_id";
}

void
ProtoJournal::do_clear_entries()
{
	(m_data->entries).clear();
	return;
}

void
ProtoJournal::mimic_core
(	Journal const& rhs,
	PhatbooksDatabaseConnection& dbc,
	optional<Id> id
)
{
	set_whether_actual(rhs.is_actual());
	set_comment(rhs.comment());
	clear_entries();
	typedef vector<Entry>::const_iterator It;
	vector<Entry> const& rentries = rhs.entries();
	if (!rentries.empty())
	{
		for (It it = rentries.begin(), end = rentries.end(); it != end; ++it)
		{
			Entry entry(dbc);
			entry.mimic(*it);
			if (id) entry.set_journal_id(value(id));
			push_entry(entry);
		}
	}
	return;
}



}  // namespace phatbooks

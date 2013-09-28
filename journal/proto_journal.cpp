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



#include "proto_journal.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_conv.hpp"
#include "transaction_type.hpp"
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/next_auto_key.hpp>
#include <sqloxx/sql_statement.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <wx/string.h>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using sqloxx::next_auto_key;
using sqloxx::SQLStatement;
using boost::numeric_cast;
using boost::optional;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using std::back_inserter;
using std::ios_base;
using std::logic_error;
using std::ostream;
using std::ostringstream;
using std::remove_copy;
using std::string;
using std::unordered_set;
using std::vector;


namespace phatbooks
{

void
ProtoJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table transaction_types"
		"("
			"transaction_type_id integer primary key"
		");"
	);
	using transaction_type::TransactionType;
	for
	(	int i = 0;
		i != static_cast<int>(transaction_type::num_transaction_types);
		++i
	)
	{
		SQLStatement statement
		(	dbc,
			"insert into transaction_types(transaction_type_id) values(:p)"
		);
		statement.bind(":p", i);
		statement.step_final();
	}
	dbc.execute_sql
	(	"create table journals"
		"("
			"journal_id integer primary key autoincrement, "
			"transaction_type_id integer not null references "
				"transaction_types, "
			"comment text"
		");"
	);
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

vector<Entry> const& 
ProtoJournal::do_get_entries() const
{
	return m_data->entries;
}

void
ProtoJournal::do_set_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	m_data->transaction_type = p_transaction_type;
	return;
}

void
ProtoJournal::do_set_comment(wxString const& p_comment)
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
		

wxString
ProtoJournal::do_get_comment() const
{
	return value(m_data->comment);
}

transaction_type::TransactionType
ProtoJournal::do_get_transaction_type() const
{
	return value(m_data->transaction_type);
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
		JEWEL_THROW
		(	UnbalancedJournalException,
			"Cannot save journal core in unbalanced state."
		);
	}
	Id const journal_id = next_auto_key
	<	PhatbooksDatabaseConnection,
		Id
	>	(dbc, "journals");
	SQLStatement statement
	(	dbc,
		"insert into journals(transaction_type_id, comment) "
		"values(:transaction_type_id, :comment)"
	);
	statement.bind
	(	":transaction_type_id",
		static_cast<int>(value(m_data->transaction_type))
	);
	statement.bind(":comment", wx_to_std8(value(m_data->comment)));
	statement.step_final();
	for (Entry& entry: m_data->entries)
	{
		entry.set_journal_id(journal_id);
		entry.save();
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
		JEWEL_THROW
		(	UnbalancedJournalException,
			"Cannot save journal core in unbalanced state."
		);
	}
	SQLStatement updater
	(	dbc,
		"update journals "
		"set comment = :comment, "
		"transaction_type_id = :transaction_type_id "
		"where journal_id = :id"
	);
	updater.bind
	(	":transaction_type_id",
		static_cast<int>(value(m_data->transaction_type))
	);
	updater.bind(":comment", wx_to_std8(value(m_data->comment)));
	updater.bind(":id", id);
	updater.step_final();
	unordered_set<Entry::Id> saved_entry_ids;
	for (Entry& entry: m_data->entries)
	{
		entry.save();
		JEWEL_ASSERT (entry.has_id());
		saved_entry_ids.insert(entry.id());
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
		"select transaction_type_id, comment "
		"from journals where journal_id = :p"
	);
	statement.bind(":p", id);
	statement.step();
	ProtoJournal temp(*this);
	SQLStatement entry_finder
	(	dbc,	
		"select entry_id from entries where journal_id = :jid "
		"order by entry_id"
	);
	entry_finder.bind(":jid", id);
	while (entry_finder.step())
	{
		Entry::Id const entr_id = entry_finder.extract<Entry::Id>(0);
		Entry entry(dbc, entr_id);
		temp.m_data->entries.push_back(entry);
	}
	temp.m_data->transaction_type =
		static_cast<transaction_type::TransactionType>
		(	statement.extract<int>(0)
		);
	temp.m_data->comment = std8_to_wx(statement.extract<string>(1));
	swap(temp);	
	return;
}

void
ProtoJournal::do_ghostify_journal_core()
{
	clear(m_data->transaction_type);
	clear(m_data->comment);
	for (Entry& entry: m_data->entries)
	{
		entry.ghostify();
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
	set_transaction_type(rhs.transaction_type());
	set_comment(rhs.comment());
	clear_entries();
	for (Entry const& rentry: rhs.entries())
	{
		Entry entry(dbc);
		entry.mimic(rentry);
		if (id) entry.set_journal_id(value(id));
		push_entry(entry);
	}
	return;
}



}  // namespace phatbooks

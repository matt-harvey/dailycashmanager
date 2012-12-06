#include "draft_journal_impl.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "repeater.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>  // for debug logging
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <string>
#include <vector>

using sqloxx::SharedSQLStatement;
using boost::shared_ptr;
using boost::unordered_set;
using jewel::clear;
using jewel::value;
using std::vector;
using std::string;

// For debug logging:
using std::endl;

namespace phatbooks
{

string
DraftJournalImpl::primary_table_name()
{
	return Journal::primary_table_name();
}

string
DraftJournalImpl::primary_key_name()
{
	return Journal::primary_key_name();
}

void
DraftJournalImpl::set_whether_actual(bool p_is_actual)
{
	load();
	Journal::set_whether_actual(p_is_actual);
	return;
}

void
DraftJournalImpl::set_comment(string const& p_comment)
{
	load();
	Journal::set_comment(p_comment);
	return;
}

void
DraftJournalImpl::add_entry(Entry& entry)
{
	load();
	if (has_id())
	{
		entry.set_journal_id(id());
	}
	Journal::add_entry(entry);
	return;
}

bool
DraftJournalImpl::is_actual()
{
	load();
	return Journal::is_actual();
}

string
DraftJournalImpl::comment()
{
	load();
	return Journal::comment();
}


vector<Entry> const&
DraftJournalImpl::entries()
{
	load();
	// WARNING Should this fail if m_entries is empty? This would
	// be the same behaviour then as the other "optionals". To be
	// truly consistent with the other optionals, it would fail
	// by means of a failed assert (assuming I haven't wrapped the
	// other optionals in some throwing construct...).
	return Journal::entries();
}



void
DraftJournalImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table draft_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"name text not null unique"
		")"
	);
	return;
}


DraftJournalImpl::DraftJournalImpl
(	IdentityMap& p_identity_map
):
	DraftJournalImpl::PersistentObject(p_identity_map),
	Journal(),
	m_dj_data(new DraftJournalData)
{
}


DraftJournalImpl::DraftJournalImpl
(	IdentityMap& p_identity_map,	
	Id p_id
):
	DraftJournalImpl::PersistentObject(p_identity_map, p_id),
	Journal(),
	m_dj_data(new DraftJournalData)
{
}



DraftJournalImpl::DraftJournalImpl
(	Journal const& p_journal,
	IdentityMap& p_identity_map
):
	DraftJournalImpl::PersistentObject(p_identity_map),
	Journal(p_journal),
	m_dj_data(new DraftJournalData)
{
}


DraftJournalImpl::~DraftJournalImpl()
{
	/* If m_dj_data is a smart pointer, this is not required.
	delete m_dj_data;
	m_dj_data = 0;
	*/
}


void
DraftJournalImpl::set_name(string const& p_name)
{
	load();
	m_dj_data->name = p_name;
	return;
}


void
DraftJournalImpl::add_repeater(Repeater& repeater)
{
	load();
	if (has_id())
	{
		repeater.set_journal_id(id());
	}
	m_dj_data->repeaters.push_back(repeater);
	return;
}



std::string
DraftJournalImpl::name()
{
	load();
	return value(m_dj_data->name);
}


DraftJournalImpl::DraftJournalImpl(DraftJournalImpl const& rhs):
	DraftJournalImpl::PersistentObject(rhs),
	Journal(rhs),
	m_dj_data(new DraftJournalData(*(rhs.m_dj_data)))
{
}


void
DraftJournalImpl::swap(DraftJournalImpl& rhs)
{
	swap_base_internals(rhs);
	Journal::swap(rhs);
	using std::swap;
	swap(m_dj_data, rhs.m_dj_data);
	return;
}


void
DraftJournalImpl::do_load()
{
	DraftJournalImpl temp(*this);
	
	// Load the base part of temp.
	temp.do_load_journal_base(database_connection(), id());

	// Load the derived, DraftJournalImpl part of the temp.
	SharedSQLStatement statement
	(	database_connection(),
		"select name from draft_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	temp.m_dj_data->name = statement.extract<string>(0);
	SharedSQLStatement repeater_finder
	(	database_connection(),
		"select repeater_id from repeaters where journal_id = :p"
	);
	repeater_finder.bind(":journal_id", id());
	while (repeater_finder.step())
	{
		Repeater::Id const rep_id =
			repeater_finder.extract<Repeater::Id>(0);
		Repeater repeater(database_connection(), rep_id);
		temp.m_dj_data->repeaters.push_back(repeater);
	}
	swap(temp);
	return;
}


void
DraftJournalImpl::do_save_new()
{
	// Save the Journal (base) part of the object
	Id const journal_id = do_save_new_journal_base(database_connection());

	// Save the derived, DraftJournalImpl part of the object
	SharedSQLStatement statement
	(	database_connection(),
		"insert into draft_journal_detail(journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":name", value(m_dj_data->name));
	statement.step_final();
	
	typedef vector<Repeater>::iterator RepIter;
	RepIter const endpoint = m_dj_data->repeaters.end();
	for (RepIter it = m_dj_data->repeaters.begin(); it != endpoint; ++it)
	{
		it->set_journal_id(journal_id);
		it->save();
	}
	return;
}

void
DraftJournalImpl::do_save_existing()
{
	do_save_existing_journal_base(database_connection(), id());
	SharedSQLStatement updater
	(	database_connection(),
		"update draft_journal_detail set name = :name where "
		"journal_id = :journal_id"
	);
	updater.bind(":journal_id", id());
	updater.bind(":name", value(m_dj_data->name));
	updater.step_final();

	typedef vector<Repeater>::iterator RepIter;
	RepIter const endpoint = m_dj_data->repeaters.end();
	unordered_set<Repeater::Id> saved_repeater_ids;
	for (RepIter it = m_dj_data->repeaters.begin(); it != endpoint; ++it)
	{
		it->save();
		saved_repeater_ids.insert(it->id());
	}
	// Now remove any repeaters in the database with this DraftJournalImpl's
	// journal_id, that no longer exist in the in-memory DraftJournalImpl
	SharedSQLStatement repeater_finder
	(	database_connection(),
		"select repeater_id from repeaters where journal_id = :journal_id"
	);
	repeater_finder.bind(":journal_id", id());
	unordered_set<Repeater::Id>::const_iterator const saved_repeaters_end =
		saved_repeater_ids.end();
	while (repeater_finder.step())
	{
		Repeater::Id const repeater_id
			= repeater_finder.extract<Repeater::Id>(0);
		if (saved_repeater_ids.find(repeater_id) == saved_repeaters_end)
		{
			// This repeater is in the database but no longer in the in-memory
			// DraftJournalImpl, and so should be deleted from the database.
			Repeater doomed_repeater(database_connection(), repeater_id);
			doomed_repeater.remove();
			// Note it's OK even if the last repeater is deleted. Another
			// repeater will never be reassigned its id - SQLite makes sure
			// of that - providing we let SQLite assign all the ids
			// automatically.
		}
	}
	return;
}


void
DraftJournalImpl::do_ghostify()
{
	do_ghostify_journal_base();
	clear(m_dj_data->name);
	typedef vector<Repeater>::iterator RepIter;
	RepIter endpoint = m_dj_data->repeaters.end();
	for (RepIter it = m_dj_data->repeaters.begin(); it != endpoint; ++it)
	{
		it->ghostify();
	}
	m_dj_data->repeaters.clear();
	return;
}




}  // namespace phatbooks




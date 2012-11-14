#include "draft_journal.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <iostream>  // for debug logging
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <string>
#include <vector>

using sqloxx::DatabaseConnection;
using sqloxx::SharedSQLStatement;
using boost::shared_ptr;
using boost::unordered_set;
using jewel::value;
using std::vector;
using std::string;

// For debug logging:
using std::endl;

namespace phatbooks
{

void
DraftJournal::setup_tables(DatabaseConnection& dbc)
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


DraftJournal::DraftJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	Journal(p_database_connection),
	m_dj_data(new DraftJournalData)
{
}


DraftJournal::DraftJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	Journal(p_database_connection, p_id),
	m_dj_data(new DraftJournalData)
{
}



DraftJournal::DraftJournal(Journal const& p_journal):
	Journal(p_journal),
	m_dj_data(new DraftJournalData)
{
}


DraftJournal::~DraftJournal()
{
	/* If m_dj_data is a smart pointer, this is not required.
	delete m_dj_data;
	m_dj_data = 0;
	*/
}


void
DraftJournal::set_name(string const& p_name)
{
	load();
	m_dj_data->name = p_name;
	return;
}


void
DraftJournal::add_repeater(shared_ptr<Repeater> repeater)
{
	load();
	if (has_id())
	{
		repeater->set_journal_id(id());
	}
	m_dj_data->repeaters.push_back(repeater);
	return;
}



std::string
DraftJournal::name()
{
	load();
	return value(m_dj_data->name);
}


DraftJournal::DraftJournal(DraftJournal const& rhs):
	Journal(rhs),
	m_dj_data(new DraftJournalData(*(rhs.m_dj_data)))
{
}


void
DraftJournal::swap(DraftJournal& rhs)
{
	Journal::swap(rhs);
	using std::swap;
	swap(m_dj_data, rhs.m_dj_data);
	return;
}


void
DraftJournal::do_load()
{
	DraftJournal temp(*this);
	
	// Load the base part of temp.
	temp.do_load_journal_base();

	// Load the derived, DraftJournal part of the temp.
	SharedSQLStatement statement
	(	*database_connection(),
		"select name from draft_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	temp.m_dj_data->name = statement.extract<string>(0);
	SharedSQLStatement repeater_finder
	(	*database_connection(),
		"select repeater_id from repeaters where journal_id = :p"
	);
	repeater_finder.bind(":journal_id", id());
	while (repeater_finder.step())
	{
		Repeater::Id const rep_id =
			repeater_finder.extract<Repeater::Id>(0);
		shared_ptr<Repeater> repeater
		(	new Repeater(database_connection(), rep_id)
		);
		temp.m_dj_data->repeaters.push_back(repeater);
	}
	swap(temp);
	return;
}


void
DraftJournal::do_save_new()
{
	// Save the Journal (base) part of the object
	Id const journal_id = do_save_new_journal_base();

	// Save the derived, DraftJournal part of the object
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into draft_journal_detail(journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":name", value(m_dj_data->name));
	statement.step_final();
	
	typedef vector< shared_ptr<Repeater> >::iterator RepIter;
	RepIter const endpoint = m_dj_data->repeaters.end();
	for (RepIter it = m_dj_data->repeaters.begin(); it != endpoint; ++it)
	{
		(*it)->set_journal_id(journal_id);
		(*it)->save_new();
	}
	return;
}

void
DraftJournal::do_save_existing()
{
	do_save_existing_journal_base();
	SharedSQLStatement updater
	(	*database_connection(),
		"update draft_journal_detail set name = :name where "
		"journal_id = :journal_id"
	);
	updater.bind(":journal_id", id());
	updater.bind(":name", value(m_dj_data->name));
	updater.step_final();

	typedef vector< shared_ptr<Repeater> >::const_iterator RepIter;
	RepIter const endpoint = m_dj_data->repeaters.end();
	unordered_set<Repeater::Id> saved_repeater_ids;
	for (RepIter it = m_dj_data->repeaters.begin(); it != endpoint; ++it)
	{
		(*it)->save();
		saved_repeater_ids.insert((*it)->id());
	}
	// Now remove any repeaters in the database with this DraftJournal's
	// journal_id, that no longer exist in the in-memory DraftJournal
	SharedSQLStatement repeater_finder
	(	*database_connection(),
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
			// DraftJournal, and so should be deleted from the database.
			SharedSQLStatement repeater_deleter
			(	*database_connection(),
				"delete from repeaters where repeater_id = :repeater_id"
			);
			repeater_deleter.bind(":repeater_id", repeater_id);
			repeater_deleter.step_final();
			// Note it's OK even if the last repeater is deleted. Another
			// repeater will never be reassigned its id - SQLite makes sure
			// of that - providing we let SQLite assign all the ids
			// automatically.
		}
	}
	return;
}


}  // namespace phatbooks



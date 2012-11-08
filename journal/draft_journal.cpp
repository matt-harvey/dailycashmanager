#include "draft_journal.hpp"
#include "general_typedefs.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>  // for debug logging
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <string>
#include <vector>

using sqloxx::DatabaseConnection;
using sqloxx::SharedSQLStatement;
using boost::shared_ptr;
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
	m_dj_data->name = p_name;
	return;
}


void
DraftJournal::add_repeater(shared_ptr<Repeater> repeater)
{
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
DraftJournal::do_load_all()
{
	DraftJournal temp(*this);
	
	// Load the base part of temp.
	temp.Journal::do_load_all();

	// Load the derived, DraftJournal part of the temp.
	SharedSQLStatement statement
	(	*database_connection(),
		"select name from draft_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	temp.set_name(statement.extract<string>(0));
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
		temp.add_repeater(repeater);
	}
	swap(temp);
	return;
}


void
DraftJournal::do_save_new_all()
{
	JEWEL_DEBUG_LOG << "Saving DraftJournal..." << endl;

	// Save the Journal (base) part of the object
	JEWEL_DEBUG_LOG << "Saving base part of DraftJournal..." << endl;
	Id const journal_id = do_save_new_all_journal_base();

	// Save the derived, DraftJournal part of the object
	JEWEL_DEBUG_LOG << "Saving derived, DraftJournal part..." << endl;
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into draft_journal_detail(journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":name", value(m_dj_data->name));
	statement.step_final();
	
	JEWEL_DEBUG_LOG << "Saving Repeaters..." << endl;
	typedef vector< shared_ptr<Repeater> >::iterator RepIter;
	RepIter const endpoint = m_dj_data->repeaters.end();
	for (RepIter it = m_dj_data->repeaters.begin(); it != endpoint; ++it)
	{
		(*it)->set_journal_id(journal_id);
		(*it)->save_new();
	}
	return;
}

}  // namespace phatbooks



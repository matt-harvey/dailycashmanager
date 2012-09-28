#include "draft_journal.hpp"
#include "general_typedefs.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>  // for debug logging
#include <jewel/debug_log.hpp>
#include <string>
#include <vector>

using sqloxx::DatabaseConnection;
using sqloxx::SharedSQLStatement;
using boost::shared_ptr;
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
	m_repeaters(vector< shared_ptr<Repeater> >())
{
}


DraftJournal::DraftJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	Journal(p_database_connection, p_id),
	m_repeaters(vector< shared_ptr<Repeater> >())
{
}


DraftJournal::DraftJournal(Journal const& p_journal):
	Journal(p_journal)
{
}


DraftJournal::~DraftJournal()
{
}


void
DraftJournal::set_name(string const& p_name)
{
	m_name = p_name;
	return;
}


void
DraftJournal::add_repeater(shared_ptr<Repeater> repeater)
{
	if (has_id())
	{
		repeater->set_journal_id(id());
	}
	m_repeaters.push_back(repeater);
	return;
}



std::string
DraftJournal::name()
{
	load();
	return *m_name;
}


void
DraftJournal::do_load_all()
{
	// Load the Journal (base) part of the object.
	Journal::do_load_all();

	// Load the derived, DraftJournal part of the object.
	SharedSQLStatement statement
	(	*database_connection(),
		"select name from draft_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	set_name(statement.extract<string>(0));
	
	// Load repeaters
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
		add_repeater(repeater);
	}
	return;
}


void
DraftJournal::do_save_new_all()
{
	JEWEL_DEBUG_LOG << "Saving DraftJournal..." << endl;

	// Save the Journal (base) part of the object
	JEWEL_DEBUG_LOG << "Saving base part of DraftJournal..." << endl;
	Id const j_id = do_save_new_all_journal_base();

	// Save the derived, DraftJournal part of the object
	JEWEL_DEBUG_LOG << "Saving derived, DraftJournal part..." << endl;
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into draft_journal_detail(journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", j_id);
	statement.bind(":name", *m_name);
	statement.step_last();
	
	JEWEL_DEBUG_LOG << "Saving Repeaters..." << endl;
	typedef vector< shared_ptr<Repeater> >::iterator RepIter;
	for (RepIter it = m_repeaters.begin(); it != m_repeaters.end(); ++it)
	{
		(*it)->set_journal_id(j_id);
		(*it)->save_new();
	}
	return;
}

}  // namespace phatbooks



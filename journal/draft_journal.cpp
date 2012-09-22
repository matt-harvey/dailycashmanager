#include "draft_journal.hpp"
#include "general_typedefs.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sql_statement.hpp"
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>

using sqloxx::DatabaseConnection;
using sqloxx::SQLStatement;
using boost::shared_ptr;
using std::list;
using std::string;

namespace phatbooks
{

void
DraftJournal::setup_tables(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table draft_journal_detail"
		"("
			"journal_id integer not null unique references journals, "
			"name text not null unique"
		")"
	);
	return;
}


DraftJournal::DraftJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	Journal(p_database_connection),
	m_repeaters(list< shared_ptr<Repeater> >())
{
}


DraftJournal::DraftJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	Journal(p_database_connection, p_id),
	m_repeaters(list< shared_ptr<Repeater> >())
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
	SQLStatement statement
	(	*database_connection(),
		"select name from draft_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	set_name(statement.extract<string>(0));
	
	// Load repeaters
	SQLStatement repeater_finder
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
	// Save the Journal (base) part of the object
	Id const journal_id = do_save_new_all_journal_base();

	// Save the derived, DraftJournal part of the object
	SQLStatement statement
	(	*database_connection(),
		"insert into draft_journal_details (journal_id, name) "
		"values(:journal_id, :name)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":name", *m_name);
	statement.quick_step();
	
	typedef list< shared_ptr<Repeater> >::iterator RepIter;
	for (RepIter it = m_repeaters.begin(); it != m_repeaters.end(); ++it)
	{
		(*it)->set_journal_id(journal_id);
		(*it)->save_new();
	}
	return;
}

}  // namespace phatbooks



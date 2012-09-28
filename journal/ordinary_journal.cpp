#include "ordinary_journal.hpp"
#include "journal.hpp"
#include "general_typedefs.hpp"
#include "date.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/cstdint.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>


using boost::numeric_cast;
using boost::shared_ptr;
using sqloxx::DatabaseConnection;
using sqloxx::SharedSQLStatement;


namespace phatbooks
{


void
OrdinaryJournal::setup_tables(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table ordinary_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"date integer not null"
		")"
	);
	return;
}


OrdinaryJournal::OrdinaryJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	Journal(p_database_connection)
{
}


OrdinaryJournal::OrdinaryJournal
(	shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	Journal(p_database_connection, p_id)
{
}
	

OrdinaryJournal::OrdinaryJournal(Journal const& p_journal):
	Journal(p_journal)
{
}


OrdinaryJournal::~OrdinaryJournal()
{
}


void
OrdinaryJournal::set_date(boost::gregorian::date const& p_date)
{
	m_date = julian_int(p_date);
	return;
}


boost::gregorian::date
OrdinaryJournal::date()
{
	load();
	return boost_date_from_julian_int(*m_date);
}


void
OrdinaryJournal::do_load_all()
{
	// Load the Journal (base) part of the object
	Journal::do_load_all();

	// Load the derived, OrdinaryJournal part of the object
	SharedSQLStatement statement
	(	*database_connection(),
		"select date from ordinary_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	m_date = numeric_cast<DateRep>(statement.extract<boost::int64_t>(0));
	return;
}


void
OrdinaryJournal::do_save_new_all()
{
	// Save the Journal	(base) part of the object and record the id.
	Id const journal_id = do_save_new_all_journal_base();

	// Save the derived, OrdinaryJournal part of the object
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into ordinary_journal_detail (journal_id, date) "
		"values(:journal_id, :date)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":date", *m_date);
	statement.step_last();
	return;
}

}  // namespace phatbooks



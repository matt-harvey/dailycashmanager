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
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <jewel/optional.hpp>


using boost::numeric_cast;
using boost::shared_ptr;
using jewel::value;
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


OrdinaryJournal::OrdinaryJournal(OrdinaryJournal const& rhs):
	Journal(rhs),
	m_date(rhs.m_date)
{
}


OrdinaryJournal::~OrdinaryJournal()
{
}


void
OrdinaryJournal::set_date(boost::gregorian::date const& p_date)
{
	load();
	m_date = julian_int(p_date);
	return;
}


boost::gregorian::date
OrdinaryJournal::date()
{
	load();
	return boost_date_from_julian_int(value(m_date));
}


void
OrdinaryJournal::swap(OrdinaryJournal& rhs)
{
	Journal::swap(rhs);
	using std::swap;
	swap(m_date, rhs.m_date);
	return;
}


void
OrdinaryJournal::do_load()
{
	OrdinaryJournal temp(*this);

	// Load the Journal (base) part of temp.
	temp.do_load_journal_base();

	// Load the derived, OrdinaryJournal part of temp.
	SharedSQLStatement statement
	(	*database_connection(),
		"select date from ordinary_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	// If this assertion ever fails, it's a reminder that the exception-safety
	// of loading here MAY depend on m_date being of a native, non-throwing
	// type.
	BOOST_STATIC_ASSERT((boost::is_same<DateRep, int>::value));
	temp.m_date = numeric_cast<DateRep>(statement.extract<boost::int64_t>(0));
	swap(temp);
	return;
}


void
OrdinaryJournal::do_save_new()
{
	// Save the Journal	(base) part of the object and record the id.
	Id const journal_id = do_save_new_journal_base();

	// Save the derived, OrdinaryJournal part of the object
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into ordinary_journal_detail (journal_id, date) "
		"values(:journal_id, :date)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":date", value(m_date));
	statement.step_final();
	return;
}

}  // namespace phatbooks



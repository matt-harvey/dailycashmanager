#include "ordinary_journal_impl.hpp"
#include "draft_journal.hpp"
#include "journal.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <jewel/optional.hpp>
#include <string>
#include <vector>

using boost::numeric_cast;
using boost::shared_ptr;
using jewel::clear;
using jewel::value;
using sqloxx::SQLStatement;
using std::string;
using std::vector;


#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;




namespace phatbooks
{


string
OrdinaryJournalImpl::primary_table_name()
{
	return Journal::primary_table_name();
}

string
OrdinaryJournalImpl::primary_key_name()
{
	return Journal::primary_key_name();
}

void
OrdinaryJournalImpl::set_whether_actual(bool p_is_actual)
{
	load();
	Journal::set_whether_actual(p_is_actual);
	return;
}

void
OrdinaryJournalImpl::set_comment(string const& p_comment)
{
	load();
	Journal::set_comment(p_comment);
	return;
}

void
OrdinaryJournalImpl::add_entry(Entry& entry)
{
	/*
	JEWEL_DEBUG_LOG << "Calling OrdinaryJournalImpl::add_entry."
	                << endl;
	*/
	load();
	if (has_id())
	{
		entry.set_journal_id(id());
	}
	Journal::add_entry(entry);
	return;
}

bool
OrdinaryJournalImpl::is_actual()
{
	load();
	return Journal::is_actual();
}

string
OrdinaryJournalImpl::comment()
{
	load();
	return Journal::comment();
}


vector<Entry> const&
OrdinaryJournalImpl::entries()
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
OrdinaryJournalImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
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


OrdinaryJournalImpl::OrdinaryJournalImpl
(	IdentityMap& p_identity_map
):
	OrdinaryJournalImpl::PersistentObject(p_identity_map),
	Journal()
{
}


OrdinaryJournalImpl::OrdinaryJournalImpl
(	IdentityMap& p_identity_map,
	Id p_id
):
	OrdinaryJournalImpl::PersistentObject(p_identity_map, p_id),
	Journal()
{
}
	

OrdinaryJournalImpl::OrdinaryJournalImpl
(	Journal const& p_journal,
	IdentityMap& p_identity_map
):
	OrdinaryJournalImpl::PersistentObject(p_identity_map),
	Journal(p_journal)
{
}


OrdinaryJournalImpl::OrdinaryJournalImpl(OrdinaryJournalImpl const& rhs):
	OrdinaryJournalImpl::PersistentObject(rhs),
	Journal(rhs),
	m_date(rhs.m_date)
{
}


OrdinaryJournalImpl::~OrdinaryJournalImpl()
{
}


void
OrdinaryJournalImpl::set_date(boost::gregorian::date const& p_date)
{
	load();
	m_date = julian_int(p_date);
	return;
}


boost::gregorian::date
OrdinaryJournalImpl::date()
{
	load();
	return boost_date_from_julian_int(value(m_date));
}


void
OrdinaryJournalImpl::swap(OrdinaryJournalImpl& rhs)
{
	swap_base_internals(rhs);
	Journal::swap(rhs);
	using std::swap;
	swap(m_date, rhs.m_date);
	return;
}

void
OrdinaryJournalImpl::clear_entries()
{
	load();
	Journal::clear_entries();
	return;
}


void
OrdinaryJournalImpl::do_load()
{
	OrdinaryJournalImpl temp(*this);

	// Load the Journal (base) part of temp.
	temp.do_load_journal_base(database_connection(), id());

	// Load the derived, OrdinaryJournalImpl part of temp.
	SQLStatement statement
	(	database_connection(),
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
OrdinaryJournalImpl::do_save_new()
{
	// Save the Journal	(base) part of the object and record the id.
	Id const journal_id = do_save_new_journal_base(database_connection());

	// Save the derived, OrdinaryJournalImpl part of the object
	SQLStatement statement
	(	database_connection(),
		"insert into ordinary_journal_detail (journal_id, date) "
		"values(:journal_id, :date)"
	);
	statement.bind(":journal_id", journal_id);
	statement.bind(":date", value(m_date));
	statement.step_final();
	return;
}

void
OrdinaryJournalImpl::do_save_existing()
{
	// Save the Journal (base) part of the object
	do_save_existing_journal_base(database_connection(), id());

	// Save the derived, OrdinaryJournalImpl part of the object
	SQLStatement updater
	(	database_connection(),	
		"update ordinary_journal_detail set date = :date "
		"where journal_id = :journal_id"
	);
	updater.bind(":date", value(m_date));
	updater.bind(":journal_id", id());
	updater.step_final();
	return;
}


void
OrdinaryJournalImpl::do_ghostify()
{
	do_ghostify_journal_base();
	clear(m_date);
	return;
}


void
OrdinaryJournalImpl::mimic(Journal& rhs)
{
	load();
	OrdinaryJournalImpl temp(*this);
	temp.mimic_core(rhs);
	swap(temp);
	return;
}

void
OrdinaryJournalImpl::mimic(DraftJournal const& rhs)
{
	load();
	OrdinaryJournalImpl temp(*this);
	temp.mimic_core(rhs);
	swap(temp);
	return;
}

void
OrdinaryJournalImpl::mimic(OrdinaryJournalImpl& rhs)
{
	load();
	OrdinaryJournalImpl temp(*this);
	temp.mimic_core(rhs);
	temp.set_date(rhs.date());
	swap(temp);
	return;
}
}  // namespace phatbooks



// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "ordinary_journal.hpp"
#include "draft_journal_handle.hpp"
#include "date.hpp"
#include "entry_handle.hpp"
#include "ordinary_journal_handle.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <jewel/signature.hpp>
#include <wx/string.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

using boost::numeric_cast;
using boost::optional;
using jewel::clear;
using jewel::Decimal;
using jewel::Signature;
using jewel::value;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::endl;
using std::is_same;
using std::string;
using std::vector;


namespace gregorian = boost::gregorian;


namespace phatbooks
{

string
OrdinaryJournal::exclusive_table_name()
{
	return "ordinary_journal_detail";
}

void
OrdinaryJournal::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table ordinary_journal_detail"
		"("
			"journal_id integer primary key references journals, "
			"date integer not null"
		"); "
	);
	dbc.execute_sql
	(	"create index journal_date_index on ordinary_journal_detail(date); "
	);
	return;
}

OrdinaryJournal::OrdinaryJournal
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentJournal(p_identity_map, p_signature)
{
}

OrdinaryJournal::OrdinaryJournal
(	IdentityMap& p_identity_map,
	Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentJournal(p_identity_map, p_id, p_signature)
{
}
		
OrdinaryJournal::OrdinaryJournal(OrdinaryJournal const& rhs):
	PersistentJournal(rhs),
	m_date(rhs.m_date)
{
}

void
OrdinaryJournal::set_date(gregorian::date const& p_date)
{
	if (p_date < database_connection().entity_creation_date())
	{
		JEWEL_THROW
		(	InvalidJournalDateException,
			"Date of OrdinaryJournal cannot be set to a date "
			"earlier than the entity creation date, using the "
			"set_date function."
		);
	}
	JEWEL_ASSERT (p_date != database_connection().opening_balance_journal_date());
	load();
	set_date_unrestricted(p_date);
	JEWEL_ASSERT
	(	boost_date_from_julian_int(value(m_date)) >=
		database_connection().entity_creation_date()
	);
	return;
}

void
OrdinaryJournal::set_date_unrestricted(gregorian::date const& p_date)
{
	load();
	m_date = julian_int(p_date);
	return;
}

gregorian::date
OrdinaryJournal::date()
{
	load();
	return boost_date_from_julian_int(value(m_date));
}

void
OrdinaryJournal::swap(OrdinaryJournal& rhs)
{
	PersistentJournal::swap(rhs);
	using std::swap;
	swap(m_date, rhs.m_date);
	return;
}

void
OrdinaryJournal::do_load()
{
	OrdinaryJournal temp(*this);

	// Load the Journal (base) part of temp.
	temp.load_journal_core();

	// Load the derived, OrdinaryJournal part of temp.
	SQLStatement statement
	(	database_connection(),
		"select date from ordinary_journal_detail where journal_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	// If this assertion ever fails, it's a reminder that the exception-safety
	// of loading here MAY depend on m_date being of a native, non-throwing
	// type.
	static_assert
	(	is_same<DateRep, int>::value,
		"DateRep needs to be int."
	);
	temp.m_date = numeric_cast<DateRep>(statement.extract<long long>(0));
	swap(temp);
	return;
}

void
OrdinaryJournal::do_save_new()
{
	// Save the Journal	(base) part of the object and record the id.
	Id const journal_id = save_new_journal_core();

	// Save the derived, OrdinaryJournal part of the object
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
OrdinaryJournal::do_save_existing()
{
	JEWEL_LOG_TRACE();

	// Save the Journal (base) part of the object
	save_existing_journal_core();
	JEWEL_LOG_TRACE();

	// Save the derived, OrdinaryJournal part of the object
	SQLStatement updater
	(	database_connection(),	
		"update ordinary_journal_detail set date = :date "
		"where journal_id = :journal_id"
	);
	updater.bind(":date", value(m_date));
	updater.bind(":journal_id", id());
	updater.step_final();
	JEWEL_LOG_TRACE();
	return;
}

void
OrdinaryJournal::do_ghostify()
{
	ghostify_journal_core();
	clear(m_date);
	return;
}

void
OrdinaryJournal::do_remove()
{
	// TODO Confirm exception safety of total remove() function
	// taking into account the below.
	SQLStatement journal_detail_deleter
	(	database_connection(),
		"delete from ordinary_journal_detail where "
		"journal_id = :p"
	);
	journal_detail_deleter.bind(":p", id());
	SQLStatement journal_master_deleter
	(	database_connection(),
		"delete from journals where journal_id = :p"
	);
	journal_master_deleter.bind(":p", id());
	clear_entries();
	save();
	journal_detail_deleter.step_final();
	journal_master_deleter.step_final();
	return;
}

void
OrdinaryJournal::mimic(Journal& rhs)
{
	load();
	OrdinaryJournal temp(*this);
	optional<Id> t_id;
	if (has_id()) t_id = id();
	temp.mimic_core(rhs, database_connection(), t_id);
	swap(temp);
	return;
}


}  // namespace phatbooks



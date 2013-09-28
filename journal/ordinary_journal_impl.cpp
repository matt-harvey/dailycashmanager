// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "ordinary_journal_impl.hpp"
#include "draft_journal.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
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
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <jewel/signature.hpp>
#include <wx/string.h>
#include <iostream>
#include <string>
#include <vector>

using boost::numeric_cast;
using boost::optional;
using boost::shared_ptr;
using jewel::clear;
using jewel::Decimal;
using jewel::Signature;
using jewel::value;
using sqloxx::SQLStatement;
using std::endl;
using std::string;
using std::vector;


namespace gregorian = boost::gregorian;


namespace phatbooks
{


string
OrdinaryJournalImpl::primary_table_name()
{
	return ProtoJournal::primary_table_name();
}

string
OrdinaryJournalImpl::exclusive_table_name()
{
	return "ordinary_journal_detail";
}

string
OrdinaryJournalImpl::primary_key_name()
{
	return ProtoJournal::primary_key_name();
}

void
OrdinaryJournalImpl::set_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	load();
	ProtoJournal::set_transaction_type(p_transaction_type);
	return;
}

void
OrdinaryJournalImpl::set_comment(wxString const& p_comment)
{
	load();
	ProtoJournal::set_comment(p_comment);
	return;
}

void
OrdinaryJournalImpl::push_entry(Entry& entry)
{
	load();
	if (has_id())
	{
		entry.set_journal_id(id());
	}
	ProtoJournal::push_entry(entry);
	return;
}


void
OrdinaryJournalImpl::remove_entry(Entry& entry)
{
	load();
	ProtoJournal::remove_entry(entry);
	return;
}

transaction_type::TransactionType
OrdinaryJournalImpl::transaction_type()
{
	load();
	return ProtoJournal::transaction_type();
}

wxString
OrdinaryJournalImpl::comment()
{
	load();
	return ProtoJournal::comment();
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
	return ProtoJournal::entries();
}



void
OrdinaryJournalImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
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


OrdinaryJournalImpl::OrdinaryJournalImpl
(	IdentityMap& p_identity_map
):
	OrdinaryJournalImpl::PersistentObject(p_identity_map),
	ProtoJournal()
{
}


OrdinaryJournalImpl::OrdinaryJournalImpl
(	IdentityMap& p_identity_map,
	Id p_id
):
	OrdinaryJournalImpl::PersistentObject(p_identity_map, p_id),
	ProtoJournal()
{
}
	

OrdinaryJournalImpl::OrdinaryJournalImpl(OrdinaryJournalImpl const& rhs):
	OrdinaryJournalImpl::PersistentObject(rhs),
	ProtoJournal(rhs),
	m_date(rhs.m_date)
{
}


OrdinaryJournalImpl::~OrdinaryJournalImpl()
{
}

void
OrdinaryJournalImpl::set_date(gregorian::date const& p_date)
{
	if (p_date < database_connection().entity_creation_date())
	{
		JEWEL_THROW
		(	InvalidJournalDateException,
			"Date of OrdinaryJournalImpl cannot be set to a date "
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
OrdinaryJournalImpl::set_date_unrestricted
(	gregorian::date const& p_date,
	Signature<OrdinaryJournal> const& p_signature
)
{
	// Silence compiler re. unused parameter. The Signature
	// is there to provide access control only; we don't actually
	// want to use this parameter for anything inside the function
	// body.
	(void)p_signature;

	load();
	set_date_unrestricted(p_date);
	return;
}

void
OrdinaryJournalImpl::set_date_unrestricted(gregorian::date const& p_date)
{
	load();
	m_date = julian_int(p_date);
	return;
}

gregorian::date
OrdinaryJournalImpl::date()
{
	load();
	return boost_date_from_julian_int(value(m_date));
}


void
OrdinaryJournalImpl::swap(OrdinaryJournalImpl& rhs)
{
	swap_base_internals(rhs);
	ProtoJournal::swap(rhs);
	using std::swap;
	swap(m_date, rhs.m_date);
	return;
}


void
OrdinaryJournalImpl::do_load()
{
	OrdinaryJournalImpl temp(*this);

	// Load the Journal (base) part of temp.
	temp.do_load_journal_core(database_connection(), id());

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
	temp.m_date = numeric_cast<DateRep>(statement.extract<long long>(0));
	swap(temp);
	return;
}


void
OrdinaryJournalImpl::do_save_new()
{
	// Save the Journal	(base) part of the object and record the id.
	Id const journal_id = do_save_new_journal_core(database_connection());

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
	do_save_existing_journal_core(database_connection(), id());

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
	do_ghostify_journal_core();
	clear(m_date);
	return;
}


void
OrdinaryJournalImpl::do_remove()
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
OrdinaryJournalImpl::mimic(Journal const& rhs)
{
	load();
	OrdinaryJournalImpl temp(*this);
	optional<Id> t_id;
	if (has_id()) t_id = id();
	temp.mimic_core(rhs, database_connection(), t_id);
	swap(temp);
	return;
}


void
OrdinaryJournalImpl::clear_entries()
{
	load();
	ProtoJournal::clear_entries();
	return;
}



}  // namespace phatbooks



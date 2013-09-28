// Copyright (c) 2013, Matthew Harvey. All rights reserved.


/** \file repeater_impl.cpp
 *
 * \brief Source file pertaining to RepeaterImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "repeater_impl.hpp"
#include "date.hpp"
#include "frequency.hpp"
#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "proto_journal.hpp"
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <jewel/assert.hpp>
#include <jewel/checked_arithmetic.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace gregorian = boost::gregorian;

using sqloxx::DatabaseTransaction;
using sqloxx::SQLStatement;
using boost::numeric_cast;
using jewel::clear;
using jewel::multiplication_is_unsafe;
using jewel::value;
using std::is_same;
using std::shared_ptr;
using std::string;
using std::vector;

// for debug logging
using std::endl;

namespace phatbooks
{


static_assert
(	is_same<DraftJournal::Id, sqloxx::Id>::value,
	"DraftJournal::Id must be same type as sqloxx::Id."
);


void
RepeaterImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	// TODO IntervalType is now to be used by classes other
	// than Repeater. The code for populating the interval_types
	// table should be moved elsewhere.
	dbc.execute_sql
	(	"create table interval_types"
		"("
			"interval_type_id integer primary key"
		");"
		"insert into interval_types(interval_type_id) values(1); "
		"insert into interval_types(interval_type_id) values(2); "
		"insert into interval_types(interval_type_id) values(3); "
		"insert into interval_types(interval_type_id) values(4);"
	);
	// TODO Should carefully swap the order of columns
	// interval_units and interval_type_id, to reflect their
	// usage.
	dbc.execute_sql
	(	"create table repeaters"
		"("
			"repeater_id integer primary key autoincrement, "
			"interval_type_id integer not null references interval_types, "
			"interval_units integer not null, "
			"next_date integer not null, "
			"journal_id integer not null references draft_journal_detail "
		");"
	);
	return;
}

RepeaterImpl::RepeaterImpl
(	IdentityMap& p_identity_map
):
	PersistentObject(p_identity_map),
	m_data(new RepeaterData)
{
}

RepeaterImpl::RepeaterImpl
(	IdentityMap& p_identity_map,	
	Id p_id
):
	PersistentObject(p_identity_map, p_id),
	m_data(new RepeaterData)
{
}

void
RepeaterImpl::set_frequency(Frequency const& p_frequency)
{
	load();
	m_data->frequency = p_frequency;
	return;
}

void
RepeaterImpl::set_next_date(boost::gregorian::date const& p_next_date)
{
	if (p_next_date < database_connection().entity_creation_date())
	{
		JEWEL_THROW
		(	InvalidRepeaterDateException,
			"Next firing date of RepeaterImpl cannot be set to a date "
			"earlier than the entity creation date."
		);
	}
	JEWEL_ASSERT
	(	p_next_date >=
		database_connection().entity_creation_date()
	);
	load();
	m_data->next_date = julian_int(p_next_date);
	return;
}


void
RepeaterImpl::set_journal_id(DraftJournal::Id p_journal_id)
{
	load();
	m_data->journal_id = p_journal_id;
	return;
}


Frequency
RepeaterImpl::frequency()
{
	load();
	return value(m_data->frequency);
}


gregorian::date
RepeaterImpl::next_date(vector<gregorian::date>::size_type n)
{
	load();
	typedef vector<gregorian::date>::size_type Size;
	using gregorian::date;
	using gregorian::date_duration;
	date ret = boost_date_from_julian_int(value(m_data->next_date));
	if (n == 0)
	{
		return ret;
	}
	Frequency const freq = value(m_data->frequency);
	// WARNING This conversion is potentially unsafe.
	Size const units = freq.num_steps();
	if (multiplication_is_unsafe(units, n))
	{
		JEWEL_THROW(UnsafeArithmeticException, "Unsafe multiplication.");
	}
	JEWEL_ASSERT (!multiplication_is_unsafe(units, n));
	Size const steps = units * n;
	switch (freq.step_type())
	{
	case interval_type::days:
		ret += gregorian::date_duration(steps);
		break;
	case interval_type::weeks:
		ret += gregorian::weeks(steps);
		break;
	case interval_type::month_ends:
		JEWEL_ASSERT ( (next_date(0) + date_duration(1)).day() == 1);
		// FALL THROUGH
	case interval_type::months:
		ret += gregorian::months(steps);
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	return ret;
}

shared_ptr<vector<gregorian::date> >
RepeaterImpl::firings_till(gregorian::date const& limit)
{
	load();
	using gregorian::date;
	shared_ptr<vector<date> > ret(new vector<date>);
	JEWEL_ASSERT (ret->empty());
	date d = next_date(0);
	typedef vector<gregorian::date>::size_type Size;
	for (Size i = 0; d <= limit; d = next_date(++i))
	{
		ret->push_back(d);
	}
	return ret;
}


OrdinaryJournal
RepeaterImpl::fire_next()
{
	load();
	DraftJournal const dj = draft_journal();
	OrdinaryJournal oj(database_connection());
	gregorian::date const next_date_elect = next_date(1);
	if 
	(	dj == database_connection().budget_instrument() &&
		dj.entries().empty()
	)
	{
		// Special case - if we're dealing with the budget instrument
		// and has no Entries, we do not cause the OrdinaryJournal to
		// be saved. We simply return it in an uninitialized state.
		// However, we still need to advance the next posting
		// date of the repeater.
		set_next_date(next_date_elect);
	}
	else
	{
		oj.mimic(dj);
		gregorian::date const old_next_date = next_date(0);
		oj.set_date(old_next_date);
		DatabaseTransaction transaction(database_connection());
		try
		{
			oj.save();
			set_next_date(next_date_elect);
			save();
			transaction.commit();
		}
		catch (std::exception&)
		{
			set_next_date(old_next_date);
			transaction.cancel();
			throw;
		}
	}
	return oj;
}



DraftJournal
RepeaterImpl::draft_journal()
{
	load();
	return DraftJournal(database_connection(), value(m_data->journal_id));
}


void
RepeaterImpl::swap(RepeaterImpl& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}


RepeaterImpl::RepeaterImpl(RepeaterImpl const& rhs):
	PersistentObject(rhs),
	m_data(new RepeaterData(*(rhs.m_data)))
{
}


void
RepeaterImpl::do_load()
{
	SQLStatement statement
	(	database_connection(),
		"select interval_units, interval_type_id, next_date, journal_id "
		"from repeaters where repeater_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	RepeaterImpl temp(*this);
	temp.m_data->frequency = Frequency
	(	statement.extract<int>(0),
		static_cast<interval_type::IntervalType>(statement.extract<int>(1))
	);
	temp.m_data->next_date =
		numeric_cast<DateRep>(statement.extract<long long>(2));
	temp.m_data->journal_id = statement.extract<DraftJournal::Id>(3);
	swap(temp);
	return;
}


void
RepeaterImpl::process_saving_statement(SQLStatement& statement)
{
	Frequency const freq = value(m_data->frequency);
	statement.bind(":interval_units", freq.num_steps());
	statement.bind
	(	":interval_type_id",
		static_cast<int>(freq.step_type())
	);
	statement.bind(":next_date", value(m_data->next_date));
	statement.bind(":journal_id", value(m_data->journal_id));
	statement.step_final();
	return;
}


void
RepeaterImpl::do_save_existing()
{
	SQLStatement updater
	(	database_connection(),
		"update repeaters set "
		"interval_units = :interval_units, "
		"interval_type_id = :interval_type_id, "
		"next_date = :next_date, "
		"journal_id = :journal_id "
		"where repeater_id = :repeater_id"
	);
	updater.bind(":repeater_id", id());
	process_saving_statement(updater);
	return;
}


void
RepeaterImpl::do_save_new()
{
	SQLStatement inserter
	(	database_connection(),
		"insert into repeaters(interval_units, interval_type_id, "
		"next_date, journal_id) values(:interval_units, "
		":interval_type_id, :next_date, :journal_id)"
	);
	process_saving_statement(inserter);
	return;
}


void
RepeaterImpl::do_ghostify()
{
	clear(m_data->frequency);
	clear(m_data->next_date);
	clear(m_data->journal_id);
	return;
}


string
RepeaterImpl::primary_table_name()
{
	return "repeaters";
}

string
RepeaterImpl::exclusive_table_name()
{
	return primary_table_name();
}

string
RepeaterImpl::primary_key_name()
{
	return "repeater_id";
}

void
RepeaterImpl::mimic(RepeaterImpl& rhs)
{
	load();
	RepeaterImpl temp(*this);
	temp.set_frequency(rhs.frequency());
	temp.set_next_date(rhs.next_date(0));
	swap(temp);
	return;
}


}  // namespace phatbooks

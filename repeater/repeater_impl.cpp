
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
#include "draft_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/checked_arithmetic.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include <jewel/debug_log.hpp>

namespace gregorian = boost::gregorian;

using sqloxx::DatabaseTransaction;
using sqloxx::SQLStatement;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::clear;
using jewel::multiplication_is_unsafe;
using jewel::value;
using std::string;
using std::vector;

// for debug logging
using std::endl;


namespace phatbooks
{

void
RepeaterImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
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


RepeaterImpl::~RepeaterImpl()
{
	/* If m_data is smart pointer, this is unnecessary.
	delete m_data;
	m_data = 0;
	*/
}

void
RepeaterImpl::set_interval_type(IntervalType p_interval_type)
{
	load();
	m_data->interval_type = p_interval_type;
	return;
}


void
RepeaterImpl::set_interval_units(int p_interval_units)
{
	load();
	m_data->interval_units = p_interval_units;
	return;
}


void
RepeaterImpl::set_next_date(boost::gregorian::date const& p_next_date)
{
	load();
	m_data->next_date = julian_int(p_next_date);
	return;
}


void
RepeaterImpl::set_journal_id(ProtoJournal::Id p_journal_id)
{
	load();
	m_data->journal_id = p_journal_id;
	return;
}


RepeaterImpl::IntervalType
RepeaterImpl::interval_type()
{
	load();
	return value(m_data->interval_type);
}


int
RepeaterImpl::interval_units()
{
	load();
	return value(m_data->interval_units);
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
	Size const units = value(m_data->interval_units);
	if (multiplication_is_unsafe(units, n))
	{
		throw UnsafeArithmeticException("Unsafe multiplication.");
	}
	assert (!multiplication_is_unsafe(units, n));
	Size const steps = units * n;
	switch (value(m_data->interval_type))
	{
	case interval_type::days:
		ret += gregorian::date_duration(steps);
		break;
	case interval_type::weeks:
		ret += gregorian::weeks(steps);
		break;
	case interval_type::month_ends:
		assert ( (next_date(0) + date_duration(1)).day() == 1);
		// FALL THROUGH
	case interval_type::months:
		ret += gregorian::months(steps);
		break;
	default:
		assert (false);
	}
	return ret;
}

shared_ptr<vector<gregorian::date> >
RepeaterImpl::firings_till(gregorian::date const& limit)
{
	load();
	using gregorian::date;
	boost::shared_ptr<vector<date> > ret(new vector<date>);
	assert (ret->empty());
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
	OrdinaryJournal oj(database_connection());
	oj.mimic(DraftJournal(database_connection(), journal_id()));
	boost::gregorian::date const old_next_date = next_date(0);
	oj.set_date(old_next_date);
	boost::gregorian::date const next_date_elect = next_date(1);
	
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
	return oj;
}



ProtoJournal::Id
RepeaterImpl::journal_id()
{
	load();
	return value(m_data->journal_id);
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
		"select interval_type_id, interval_units, next_date, journal_id "
		"from repeaters where repeater_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	RepeaterImpl temp(*this);
	temp.m_data->interval_type =
		static_cast<IntervalType>(statement.extract<int>(0));
	temp.m_data->interval_units = statement.extract<int>(1);
	temp.m_data->next_date =
		numeric_cast<DateRep>(statement.extract<long long>(2));
	temp.m_data->journal_id = statement.extract<ProtoJournal::Id>(3);
	swap(temp);
	return;
}


void
RepeaterImpl::process_saving_statement(SQLStatement& statement)
{
	statement.bind
	(	":interval_type_id",
		static_cast<int>(value(m_data->interval_type))
	);
	statement.bind(":interval_units", value(m_data->interval_units));
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
		"interval_type_id = :interval_type_id, "
		"interval_units = :interval_units, "
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
	// WARNING temp try catch
	try
	{
		SQLStatement inserter
		(	database_connection(),
			"insert into repeaters(interval_type_id, interval_units, "
			"next_date, journal_id) values(:interval_type_id, :interval_units, "
			":next_date, :journal_id)"
		);
		process_saving_statement(inserter);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << id() << " " << interval_units() << " " << interval_type() << std::endl;
	}
	return;
}


void
RepeaterImpl::do_ghostify()
{
	clear(m_data->interval_type);
	clear(m_data->interval_units);
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
RepeaterImpl::primary_key_name()
{
	return "repeater_id";
}

}  // namespace phatbooks

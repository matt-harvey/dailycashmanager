
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
#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/optional.hpp>
#include <algorithm>
#include <string>

using sqloxx::SharedSQLStatement;
using boost::numeric_cast;
using boost::shared_ptr;
using jewel::value;
using std::string;

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
			"interval_type_id integer primary key autoincrement, "
			"name text not null unique"
		");"
		"insert into interval_types(name) values('days'); "
		"insert into interval_types(name) values('weeks'); "
		"insert into interval_types(name) values('months'); "
		"insert into interval_types(name) values('month ends'); "
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
RepeaterImpl::set_journal_id(Journal::Id p_journal_id)
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


boost::gregorian::date
RepeaterImpl::next_date()
{
	load();
	return boost_date_from_julian_int(value(m_data->next_date));
}


Journal::Id
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
	SharedSQLStatement statement
	(	database_connection(),
		"select interval_type_id, interval_units, next_date, journal_id "
		"from repeaters where repeater_id = :p"
	);
	statement.step();
	RepeaterImpl temp(*this);
	temp.m_data->interval_type =
		static_cast<IntervalType>(statement.extract<int>(0));
	temp.m_data->interval_units = statement.extract<int>(1);
	temp.m_data->next_date =
		numeric_cast<DateRep>(statement.extract<boost::int64_t>(2));
	temp.m_data->journal_id = statement.extract<Journal::Id>(3);
	swap(temp);
	return;
}


void
RepeaterImpl::process_saving_statement(SharedSQLStatement& statement)
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
	SharedSQLStatement updater
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
	SharedSQLStatement inserter
	(	database_connection(),
		"insert into repeaters(interval_type_id, interval_units, "
		"next_date, journal_id) values(:interval_type_id, :interval_units, "
		":next_date, :journal_id)"
	);
	process_saving_statement(inserter);
	return;
}


string
RepeaterImpl::primary_table_name()
{
	return "repeaters";
}

}  // namespace phatbooks

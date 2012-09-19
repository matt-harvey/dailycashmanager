
/** \file repeater.cpp
 *
 * \brief Source file pertaining to Repeater class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "repeater.hpp"
#include "date.hpp"
#include "journal.hpp"
#include "sqloxx/sql_statement.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

using sqloxx::DatabaseConnection;
using sqloxx::SQLStatement;
using boost::numeric_cast;
using boost::shared_ptr;
using std::string;

namespace phatbooks
{

void
Repeater::setup_tables(DatabaseConnection& dbc)
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
			"next_date integer not null, "
			"journal_id integer not null references journals"
		");"
	);
	return;
}

Repeater::Repeater(shared_ptr<DatabaseConnection> p_database_connection):
	PersistentObject(p_database_connection)
{
}


Repeater::Repeater
(	shared_ptr<DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id)
{
}


void
Repeater::set_interval_type(IntervalType p_interval_type)
{
	m_interval_type = p_interval_type;
	return;
}


void
Repeater::set_interval_units(int p_interval_units)
{
	m_interval_units = p_interval_units;
	return;
}


void
Repeater::set_next_date(boost::gregorian::date const& p_next_date)
{
	m_next_date = julian_int(p_next_date);
	return;
}


void
Repeater::set_journal_id(Journal::Id p_journal_id)
{
	m_journal_id = p_journal_id;
	return;
}


Repeater::IntervalType
Repeater::interval_type()
{
	load();
	return *m_interval_type;
}


int
Repeater::interval_units()
{
	load();
	return *m_interval_units;
}


boost::gregorian::date
Repeater::next_date()
{
	load();
	return boost_date_from_julian_int(*m_next_date);
}


Journal::Id
Repeater::journal_id()
{
	load();
	return *m_journal_id;
}


void
Repeater::do_load_all()
{
	SQLStatement statement
	(	*database_connection(),
		"select interval_type_id, interval_units, next_date, journal_id "
		"from repeaters where repeater_id = :p"
	);
	statement.step();
	IntervalType const itp =
		static_cast<IntervalType>(statement.extract<int>(0));
	int const units = statement.extract<int>(1);
	DateRep const nd =
		numeric_cast<DateRep>(statement.extract<boost::int64_t>(2));
	Journal::Id const jid = statement.extract<Journal::Id>(3);
	set_interval_type(itp);
	set_interval_units(units);
	m_next_date = nd;
	set_journal_id(jid);
	return;
}


void
Repeater::do_save_new_all()
{
	SQLStatement statement
	(	*database_connection(),
		"insert into repeaters(interval_type_id, interval_units, "
		"next_date, journal_id) values(:interval_type_id, :interval_units, "
		":next_date, :journal_id)"
	);
	statement.bind(":interval_type_id", static_cast<int>(*m_interval_type));
	statement.bind(":interval_units", *m_interval_units);
	statement.bind(":next_date", *m_next_date);
	statement.bind(":journal_id", *m_journal_id);
	statement.quick_step();
	return;
}


string
Repeater::do_get_table_name()
{
	return "repeaters";
}

}  // namespace phatbooks

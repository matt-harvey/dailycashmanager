#include "commodity.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <jewel/decimal.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <algorithm>
#include <stdexcept>
#include <string>

/** \file commodity.cpp
 *
 * \brief Source file pertaining to Commodity class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


using sqloxx::DatabaseConnection;
using sqloxx::DatabaseException;
using sqloxx::SharedSQLStatement;
using jewel::Decimal;
using boost::numeric_cast;
using std::exception;
using std::string;

namespace phatbooks
{

void Commodity::setup_tables
(	DatabaseConnection& dbc
)
{
	SharedSQLStatement statement
	(	dbc,
		"create table commodities"
		"("
			"commodity_id integer primary key autoincrement, "
			"abbreviation text not null unique, "
			"name text not null unique, "
			"description text, "
			"precision integer default 2 not null, "
			"multiplier_to_base_intval integer not null, "
			"multiplier_to_base_places integer not null"
		")"
	);
	statement.step_final();
	return;
}



Commodity::Commodity
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	PersistentObject(p_database_connection),
	m_data(new CommodityData)
{
}


Commodity::Commodity
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_data(new CommodityData)
{
	load_abbreviation_knowing_id();
}


Commodity::Commodity
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	std::string const& p_abbreviation
):
	PersistentObject(p_database_connection),
	m_data(new CommodityData)
{
	m_data->abbreviation = p_abbreviation;
	load_id_knowing_abbreviation();
}


void
Commodity::swap(Commodity& rhs)
{
	swap_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}


void Commodity::load_abbreviation_knowing_id()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select abbreviation from commodities where commodity_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	set_abbreviation(statement.extract<string>(0));
	return;
}


void Commodity::load_id_knowing_abbreviation()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select commodity_id from commodities where "
		"abbreviation = :p"
	);
	statement.bind(":p", m_data->abbreviation);
	statement.step();
	set_id(statement.extract<Id>(0));
	return;
}
	

void Commodity::do_load_all()
{
	try
	{
		Commodity temp(*this);
		SharedSQLStatement statement
		(	*database_connection(),
			"select abbreviation, name, description, precision, "
			"multiplier_to_base_intval, multiplier_to_base_places from "
			"commodities where commodity_id = :p"
		);
		statement.bind(":p", id());
		statement.step();
		temp.set_abbreviation(statement.extract<string>(0));
		temp.set_name(statement.extract<string>(1));
		temp.set_description(statement.extract<string>(2));
		temp.set_precision(statement.extract<int>(3));
		temp.set_multiplier_to_base
		(	Decimal
			(	statement.extract<Decimal::int_type>(4),
				numeric_cast<Decimal::places_type>
				(	statement.extract<int>(5)
				)
			)
		);
		swap(temp);
	}
	catch (exception&)
	{
		clear_loading_status();
		throw;
	}
	return;
}


void Commodity::do_save_new_all()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"insert into commodities(abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places) "
		"values(:abbreviation, :name, :description, :precision, "
		":multiplier_to_base_intval, :multiplier_to_base_places)"
	);
	statement.bind(":abbreviation", m_data->abbreviation);
	statement.bind(":name", *(m_data->name));
	statement.bind(":description", *(m_data->description));
	statement.bind(":precision", *(m_data->precision));
	statement.bind
	(	":multiplier_to_base_intval",
		m_data->multiplier_to_base->intval()
	);
	statement.bind
	(	":multiplier_to_base_places",
		m_data->multiplier_to_base->places()
	);
	statement.step_final();	
	return;
}


std::string Commodity::abbreviation()
{
	// load not done as m_abbreviation is always initialized.
	return m_data->abbreviation;
}

std::string Commodity::name()
{
	load();
	return *(m_data->name);
}

std::string Commodity::description()
{
	load();
	return *(m_data->description);
}

int Commodity::precision()
{
	load();
	return *(m_data->precision);
}

jewel::Decimal Commodity::multiplier_to_base()
{
	load();
	return *(m_data->multiplier_to_base);
}

void Commodity::set_abbreviation(std::string const& p_abbreviation)
{
	m_data->abbreviation = p_abbreviation;
	return;
}

void Commodity::set_name(std::string const& p_name)
{
	m_data->name = p_name;
	return;
}

void Commodity::set_description(std::string const& p_description)
{
	m_data->description = p_description;
	return;
}

void Commodity::set_precision(int p_precision)
{
	m_data->precision = p_precision;
	return;
}

void Commodity::set_multiplier_to_base
(	jewel::Decimal const& p_multiplier_to_base
)
{
	m_data->multiplier_to_base = p_multiplier_to_base;
	return;
}

std::string Commodity::do_get_table_name() const
{
	return "commodities";
}


	

}  // namespace phatbooks

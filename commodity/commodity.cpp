#include "commodity.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <jewel/decimal.hpp>
#include <boost/numeric/conversion/cast.hpp>
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
	statement.bind(":p", m_abbreviation);
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
		// WARNING This could throw!
		*this = temp;	
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
	statement.bind(":abbreviation", m_abbreviation);
	statement.bind(":name", *m_name);
	statement.bind(":description", *m_description);
	statement.bind(":precision", *m_precision);
	statement.bind
	(	":multiplier_to_base_intval",
		m_multiplier_to_base->intval()
	);
	statement.bind
	(	":multiplier_to_base_places",
		m_multiplier_to_base->places()
	);
	statement.step_final();	
	return;
}


	

}  // namespace phatbooks

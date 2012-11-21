#include "commodity_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <algorithm>
#include <stdexcept>
#include <string>

/** \file commodity_impl.cpp
 *
 * \brief Source file pertaining to CommodityImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


using sqloxx::SharedSQLStatement;
using jewel::Decimal;
using jewel::value;
using boost::numeric_cast;
using boost::shared_ptr;
using std::exception;
using std::string;


namespace phatbooks
{

void CommodityImpl::setup_tables
(	PhatbooksDatabaseConnection& dbc
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


CommodityImpl::Id
CommodityImpl::id_for_abbreviation
(	PhatbooksDatabaseConnection& dbc,
	std::string const& p_abbreviation
)
{
	SharedSQLStatement statement
	(	dbc,
		"select commodity_id from commodities where abbreviation = :p"
	);
	statement.bind(":p", p_abbreviation);
	statement.step();
	Id const ret = statement.extract<Id>(0);
	statement.step_final();
	return ret;
}



CommodityImpl::CommodityImpl(CommodityImpl const& rhs):
	PersistentObject(rhs),
	m_data(new CommodityData(*(rhs.m_data)))
{
}


CommodityImpl::CommodityImpl
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection
):
	PersistentObject(p_database_connection),
	m_data(new CommodityData)
{
}


CommodityImpl::CommodityImpl
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id),
	m_data(new CommodityData)
{
	load_abbreviation_knowing_id();
}


CommodityImpl::~CommodityImpl()
{
	/* If m_data is a smart pointer, this is not required.
	delete m_data;
	m_data = 0;
	*/
}


void
CommodityImpl::swap(CommodityImpl& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}


void CommodityImpl::load_abbreviation_knowing_id()
{
	SharedSQLStatement statement
	(	*database_connection(),
		"select abbreviation from commodities where commodity_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	m_data->abbreviation = statement.extract<string>(0);
	return;
}

	

void CommodityImpl::do_load()
{
	CommodityImpl temp(*this);
	SharedSQLStatement statement
	(	*database_connection(),
		"select abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places from "
		"commodities where commodity_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	temp.m_data->abbreviation = statement.extract<string>(0);
	temp.m_data->name = statement.extract<string>(1);
	temp.m_data->description = statement.extract<string>(2);
	temp.m_data->precision = statement.extract<int>(3);
	temp.m_data->multiplier_to_base = Decimal
	(	statement.extract<Decimal::int_type>(4),
		numeric_cast<Decimal::places_type>(statement.extract<int>(5))
	);
	swap(temp);
	return;
}


void CommodityImpl::process_saving_statement(SharedSQLStatement& statement)
{
	statement.bind(":abbreviation", m_data->abbreviation);
	statement.bind(":name", value(m_data->name));
	statement.bind(":description", value(m_data->description));
	statement.bind(":precision", value(m_data->precision));
	Decimal m = value(m_data->multiplier_to_base);
	statement.bind(":multiplier_to_base_intval", m.intval());
	statement.bind(":multiplier_to_base_places", m.places());
	statement.step_final();	
	return;
}


void CommodityImpl::do_save_existing()
{
	SharedSQLStatement updater
	(	*database_connection(),
		"update commodities set "
		"abbreviation = :abbreviation, "
		"name = :name, "
		"description = :description, "
		"precision = :precision, "
		"multiplier_to_base_intval = :multiplier_to_base_intval, "
		"multiplier_to_base_places = :multiplier_to_base_places "
		"where commodity_id = :commodity_id"
	);
	updater.bind(":commodity_id", id());
	process_saving_statement(updater);
	return;
}
	

void CommodityImpl::do_save_new()
{
	SharedSQLStatement inserter
	(	*database_connection(),
		"insert into commodities(abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places) "
		"values(:abbreviation, :name, :description, :precision, "
		":multiplier_to_base_intval, :multiplier_to_base_places)"
	);
	process_saving_statement(inserter);
	return;
}


std::string CommodityImpl::abbreviation()
{
	// load not done as m_abbreviation is always initialized.
	return m_data->abbreviation;
}

std::string CommodityImpl::name()
{
	load();
	return value(m_data->name);
}

std::string CommodityImpl::description()
{
	load();
	return value(m_data->description);
}

int CommodityImpl::precision()
{
	load();
	return value(m_data->precision);
}

jewel::Decimal CommodityImpl::multiplier_to_base()
{
	load();
	return value(m_data->multiplier_to_base);
}

void CommodityImpl::set_abbreviation(std::string const& p_abbreviation)
{
	load();
	m_data->abbreviation = p_abbreviation;
	return;
}

void CommodityImpl::set_name(std::string const& p_name)
{
	load();
	m_data->name = p_name;
	return;
}

void CommodityImpl::set_description(std::string const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void CommodityImpl::set_precision(int p_precision)
{
	load();
	m_data->precision = p_precision;
	return;
}

void CommodityImpl::set_multiplier_to_base
(	jewel::Decimal const& p_multiplier_to_base
)
{
	load();
	m_data->multiplier_to_base = p_multiplier_to_base;
	return;
}


std::string CommodityImpl::primary_table_name()
{
	return "commodities";
}


	

}  // namespace phatbooks
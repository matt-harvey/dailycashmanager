// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "commodity.hpp"
#include "balance_cache.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_conv.hpp"
#include "commodity_handle.hpp"
#include "string_conv.hpp"
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <sqloxx/sql_statement.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <wx/string.h>
#include <algorithm>
#include <iostream>  // for debug logging
#include <stdexcept>
#include <string>

/** \file commodity_impl.cpp
 *
 * \brief Source file pertaining to Commodity class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

using boost::numeric_cast;
using boost::optional;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using sqloxx::DatabaseTransaction;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::endl;  // for debug logging
using std::exception;
using std::string;

namespace phatbooks
{


struct Commodity::CommodityData
{
	optional<wxString> abbreviation;
	optional<wxString> name;
	optional<wxString> description;
	optional<int> precision;
	optional<jewel::Decimal> multiplier_to_base;
};


void Commodity::setup_tables
(	PhatbooksDatabaseConnection& dbc
)
{
	// Create the table
	SQLStatement statement
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

Id
Commodity::id_for_abbreviation
(	PhatbooksDatabaseConnection& dbc,
	wxString const& p_abbreviation
)
{
	SQLStatement statement
	(	dbc,
		"select commodity_id from commodities where abbreviation = :p"
	);
	statement.bind(":p", wx_to_std8(p_abbreviation));
	statement.step();
	Id const ret = statement.extract<Id>(0);
	statement.step_final();
	return ret;
}



Commodity::Commodity(Commodity const& rhs):
	PersistentObject(rhs),
	m_data(new CommodityData(*(rhs.m_data)))
{
}


Commodity::Commodity
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map),
	m_data(new CommodityData)
{
	(void)p_signature;  // silence compiler re. unused parameter
}


Commodity::Commodity
(	IdentityMap& p_identity_map,	
	Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map, p_id),
	m_data(new CommodityData)
{
	(void)p_signature;  // silence compiler. re. unused parameter
}

Commodity::~Commodity() = default;

bool
Commodity::exists_with_abbreviation
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_abbreviation
)
{
	SQLStatement statement
	(	p_database_connection,
		"select abbreviation from commodities where abbreviation = :p"
	);
	statement.bind(":p", wx_to_std8(p_abbreviation));
	return statement.step();
}


bool
Commodity::exists_with_name
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
)
{
	SQLStatement statement
	(	p_database_connection,
		"select name from commodities where name = :p"
	);
	statement.bind(":p", wx_to_std8(p_name));
	return statement.step();
}



void
Commodity::swap(Commodity& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}


void Commodity::do_load()
{
	Commodity temp(*this);
	SQLStatement statement
	(	database_connection(),
		"select abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places from "
		"commodities where commodity_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	temp.m_data->abbreviation = std8_to_wx(statement.extract<string>(0));
	temp.m_data->name = std8_to_wx(statement.extract<string>(1));
	temp.m_data->description = std8_to_wx(statement.extract<string>(2));
	temp.m_data->precision = statement.extract<int>(3);
	temp.m_data->multiplier_to_base = Decimal
	(	statement.extract<Decimal::int_type>(4),
		numeric_cast<Decimal::places_type>(statement.extract<int>(5))
	);
	swap(temp);
	return;
}


void Commodity::process_saving_statement(SQLStatement& statement)
{
	statement.bind
	(	":abbreviation",
		wx_to_std8(value(m_data->abbreviation))
	);
	statement.bind(":name", wx_to_std8(value(m_data->name)));
	statement.bind
	(	":description",
		wx_to_std8(value(m_data->description))
	);
	statement.bind(":precision", value(m_data->precision));
	Decimal m = value(m_data->multiplier_to_base);
	statement.bind(":multiplier_to_base_intval", m.intval());
	statement.bind(":multiplier_to_base_places", m.places());
	statement.step_final();	
	return;
}


void Commodity::do_save_existing()
{
	PhatbooksDatabaseConnection::BalanceCacheAttorney::mark_as_stale
	(	database_connection()
	);
	SQLStatement updater
	(	database_connection(),
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
	

void Commodity::do_save_new()
{
	PhatbooksDatabaseConnection::BalanceCacheAttorney::mark_as_stale
	(	database_connection()
	);
	SQLStatement inserter
	(	database_connection(),
		"insert into commodities(abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places) "
		"values(:abbreviation, :name, :description, :precision, "
		":multiplier_to_base_intval, :multiplier_to_base_places)"
	);
	process_saving_statement(inserter);
	return;
}

void Commodity::do_ghostify()
{
	clear(m_data->abbreviation);
	clear(m_data->name);
	clear(m_data->description);
	clear(m_data->precision);
	clear(m_data->multiplier_to_base);
	return;
}

wxString Commodity::abbreviation()
{
	load();
	return value(m_data->abbreviation);
}

wxString Commodity::name()
{
	load();
	return value(m_data->name);
}

wxString Commodity::description()
{
	load();
	return value(m_data->description);
}

int Commodity::precision()
{
	load();
	return value(m_data->precision);
}

jewel::Decimal Commodity::multiplier_to_base()
{
	load();
	return value(m_data->multiplier_to_base);
}

void Commodity::set_abbreviation(wxString const& p_abbreviation)
{
	load();
	m_data->abbreviation = p_abbreviation;
	return;
}

void Commodity::set_name(wxString const& p_name)
{
	load();
	m_data->name = p_name;
	return;
}

void Commodity::set_description(wxString const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void Commodity::set_precision(int p_precision)
{
	load();
	m_data->precision = p_precision;
	return;
}

void Commodity::set_multiplier_to_base
(	jewel::Decimal const& p_multiplier_to_base
)
{
	load();
	m_data->multiplier_to_base = p_multiplier_to_base;
	return;
}


std::string Commodity::primary_table_name()
{
	return "commodities";
}

std::string Commodity::exclusive_table_name()
{
	return primary_table_name();
}

std::string Commodity::primary_key_name()
{
	return "commodity_id";
}


	

}  // namespace phatbooks

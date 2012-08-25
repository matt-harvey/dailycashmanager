#ifndef GUARD_commodity_storage_manager_hpp
#define GUARD_commodity_storage_manager_hpp

/** \file commodity_storage_manager.hpp
 *
 * \brief Provides code for managing the storage and retrieval
 * of Commodity object data in and from a sqloxx::DatabaseConnection.
 *
 * \author Matthew Harvey
 * \date 26 Aug 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "commodity.hpp"
#include "database_connection.hpp"
#include "storage_manager.hpp"
#include <jewel/decimal.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace sqloxx
{

/**
 * Specializes the StorageManager class template for
 * phatbooks::Commodity.
 * 
 * @todo Finish the implementation. Then use this
 * to simplify the code in the PhatbooksDatabaseConnection
 * class. In the end, I may not need PhatbooksDatabaseConnection
 * at all. We'll see...
 */
template <>
class StorageManager<phatbooks::Commodity>
{
public:
	typedef std::string Key;
	static void save(phatbooks::Commodity const& commodity, DatabaseConnection&);
	/**
	 * @todo This needs to throw if there is no Commodity
	 * with this key.
	 */
	static phatbooks::Commodity load(Key const& abbreviation, DatabaseConnection&);
	static void setup_tables(DatabaseConnection&);
};


inline
void StorageManager<phatbooks::Commodity>::save
(	phatbooks::Commodity const& obj,
	DatabaseConnection& dbc
)
{
	DatabaseConnection::SQLStatement statement
	(	dbc,
		"insert into commodities(abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places) "
		"values(:abbreviation, :name, :description, :precision, "
		":multiplier_to_base_intval, :multiplier_to_base_places)"
	);
	statement.bind(":abbreviation", obj.abbreviation());
	statement.bind(":name", obj.name());
	statement.bind(":description", obj.description());
	statement.bind(":precision", obj.precision());
	statement.bind
	(	":multiplier_to_base_intval",
		obj.multiplier_to_base().intval()
	);
	statement.bind
	(	":multiplier_to_base_places",
		obj.multiplier_to_base().places()
	);
	statement.quick_step();	
	return;
}

inline
phatbooks::Commodity StorageManager<phatbooks::Commodity>::load
(	StorageManager<phatbooks::Commodity>::Key const& abbreviation,
	DatabaseConnection& dbc
)
{
	DatabaseConnection::SQLStatement statement
	(	dbc,
		"select from commodities abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places from "
		"commodities where abbreviation = :p"
	);
	statement.bind(":p", abbreviation);
	statement.step();
	return phatbooks::Commodity
	(	statement.extract<std::string>(0),
		statement.extract<std::string>(1),
		statement.extract<std::string>(2),
		statement.extract<int>(3),
		jewel::Decimal
		(	statement.extract<jewel::Decimal::int_type>(4),
			boost::numeric_cast<jewel::Decimal::places_type>
			(	statement.extract<boost::int64_t>(5)
			)
		)
	);
}

inline
void StorageManager<phatbooks::Commodity>::setup_tables
(	DatabaseConnection& dbc
)
{
	DatabaseConnection::SQLStatement statement
	(	dbc,
		"create table commodities"
		"("
			"commodity_id integer primary key autoincrement, "
			"abbreviation text not null unique, "
			"name text unique, "
			"description text, "
			"precision integer default 2 not null, "
			"multiplier_to_base_intval integer not null, "
			"multiplier_to_base_places integer not null"
		")"
	);
	statement.quick_step();
	return;
}



}  // namespace sqloxx

#endif  // GUARD_commodity_storage_manager_hpp

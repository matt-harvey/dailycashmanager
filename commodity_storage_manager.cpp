#include "commodity.hpp"
#include "commodity_storage_manager.hpp"


namespace sqloxx
{

using phatbooks::Commodity;


void StorageManager<Commodity>::save
(	Commodity const& obj,
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

Commodity StorageManager<Commodity>::load
(	StorageManager<Commodity>::Key const& abbreviation,
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
	return Commodity
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

void StorageManager<Commodity>::setup_tables
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



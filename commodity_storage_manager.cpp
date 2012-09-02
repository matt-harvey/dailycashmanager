#include "commodity.hpp"
#include "commodity_storage_manager.hpp"
#include "sql_statement.hpp"
#include <jewel/decimal.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <string>

using phatbooks::Commodity;
using jewel::Decimal;
using boost::numeric_cast;
using std::string;


namespace sqloxx
{


void StorageManager<Commodity>::save
(	Commodity const& obj,
	DatabaseConnection& dbc
)
{
	SQLStatement statement
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
	SQLStatement statement
	(	dbc,
		"select from commodities abbreviation, name, description, precision, "
		"multiplier_to_base_intval, multiplier_to_base_places from "
		"commodities where abbreviation = :p"
	);
	statement.bind(":p", abbreviation);
	statement.step();
	return Commodity
	(	statement.extract<string>(0),
		statement.extract<string>(1),
		statement.extract<string>(2),
		statement.extract<int>(3),
		Decimal
		(	statement.extract<Decimal::int_type>(4),
			numeric_cast<Decimal::places_type>
			(	statement.extract<boost::int64_t>(5)
			)
		)
	);
}

void StorageManager<Commodity>::setup_tables
(	DatabaseConnection& dbc
)
{
	SQLStatement statement
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



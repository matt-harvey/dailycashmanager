#ifndef GUARD_commodity_storage_manager_hpp
#define GUARD_commodity_storage_manager_hpp

#include "commodity.hpp"
#include "database_connection.hpp"
#include "storage_manager.hpp"

namespace sqloxx
{

template <>
class StorageManager<phatbooks::Commodity>
{
	typedef std::string Key;
public:
	StorageManager(DatabaseConnection& p_database_connection);
	void save(phatbooks::Commodity const& commodity);
	phatbooks::Commodity load(Key key);
	static void setup_table();
	static std::string table_name();
	static std::string key_name();
private:
	DatabaseConnection& m_database_connection;
};


inline
StorageManager<phatbooks::Commodity>::StorageManager
(	DatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection)
{
}


inline
void StorageManager<phatbooks::Commodity>::save
(	phatbooks::Commodity const& obj
)
{
	DatabaseConnection::SQLStatement statement
	(	m_database_connection,
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



}  // namespace sqloxx

#endif  // GUARD_commodity_storage_manager_hpp

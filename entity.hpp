#ifndef GUARD_entity_hpp
#define GUARD_entity_hpp

#include "general_typedefs.hpp"
#include "database.hpp"
#include <string>


namespace phatbooks
{

class Entity
{
public:
protected:
private:
	Database m_database;
	std::string m_name;
	IdType m_commodity_id;  // native commodity (currency) of the entity
};


}  // namespace phatbooks

#endif  // GUARD_entity_hpp

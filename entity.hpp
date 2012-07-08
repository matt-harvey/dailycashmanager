#ifndef GUARD_entity_hpp
#define GUARD_entity_hpp

#include "general_typedefs.hpp"
#include <jewel/decimal.hpp>
#include <string>


namespace phatbooks
{

class Entity
{
public:
protected:
private:

	std::string m_name;

	std::string m_database_filepath;

	// Native commodity (currency) of the entity
	IdType m_commodity_id;
	
	// Provides conversion rates to convert between native and non-native
	// commodities. Commodities are referenced by their m_commodity_id.
	std::map<IdType, jewel::Decimal> m_conversion_table;
};


}  // namespace phatbooks

#endif  // GUARD_entity_hpp

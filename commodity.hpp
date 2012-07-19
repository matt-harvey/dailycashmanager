#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include "general_typedefs.hpp"
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

class Commodity
{
public:
private:
	std::string m_abbreviation;
	std::string m_name;
	std::string m_description;
	int m_precision;
	jewel::Decimal m_multiplier_to_base;
};



}  // namespace phatbooks

#endif  // GUARD_commodity_hpp

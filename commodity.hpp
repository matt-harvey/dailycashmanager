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

	Commodity
	(	std::string p_abbreviation,
		std::string p_name = "",
		std::string p_description = "",
		int p_precision = 2,
		jewel::Decimal p_multiplier_to_base = jewel::Decimal("1")
	);
	
	std::string abbreviation() const;
	std::string name() const;
	std::string description() const;
	int precision() const;
	jewel::Decimal multiplier_to_base() const;

private:
	std::string m_abbreviation;
	std::string m_name;
	std::string m_description;
	int m_precision;
	jewel::Decimal m_multiplier_to_base;
};



}  // namespace phatbooks

#endif  // GUARD_commodity_hpp

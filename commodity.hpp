#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include "general_typedefs.hpp"
#include <string>

namespace phatbooks
{

class Commodity
{
public:
private:
	std::string m_name;
	std::string m_abbreviation;
	IdType m_id;
};



}  // namespace phatbooks

#endif  // GUARD_commodity_hpp

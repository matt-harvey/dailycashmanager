#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

#include "general_typedefs.hpp"
#include <string>
#include <jewel/decimal.hpp>

namespace phatbooks
{

class Entry
{
private:
	DateType m_date;
	std::string m_comment;
	jewel::Decimal m_act_impact;
	jewel::Decimal m_bud_impact;
};


}  // namespace phatbooks

#endif  // GUARD_entry_hpp

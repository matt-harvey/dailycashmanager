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
	enum Status
	{
		working,
		saved,
		posted
	};
	DateType m_date;
	jewel::Decimal m_amount;
	std::string m_comment;
	IdType m_journal_id;
	IdType m_id;
};


}  // namespace phatbooks

#endif  // GUARD_entry_hpp

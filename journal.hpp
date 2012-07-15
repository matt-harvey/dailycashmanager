#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

#include "general_typedefs.hpp"
#include <vector>

namespace phatbooks
{

class Journal
{
private:
	enum Status
	{
		working,
		saved,
		posted
	};
	Status m_status;
	DateType m_date;	
	std::string m_comment;
	std::vector<IdType> m_entry_ids;
	IdType m_id;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp

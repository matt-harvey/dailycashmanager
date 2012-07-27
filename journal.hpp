#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

#include "entry.hpp"
#include "general_typedefs.hpp"
#include "repeater.hpp"
#include <vector>

namespace phatbooks
{

class Journal
{
private:
	DateType m_date;	
	std::string m_comment;
	std::vector<Entry> m_entry_vec;
	std::vector<Repeater> m_repeater_vec;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp

#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

#include "general_typedefs.hpp"
#include <list>
#include <string>

namespace phatbooks
{

class Entry;
class Repeater;

class Journal
{
public:

	/**
	 * Constructor initializes journal date
	 * to null date and journal comment to empty string,
	 * The journal starts out with an empty list of entries
	 * and an empty list of repeaters.
	 * 
	 * Does not throw.
	 */
	Journal();


private:
	DateType m_date;	
	std::string m_comment;
	std::list<Entry> m_entry_list;
	std::list<Repeater> m_repeater_list;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp

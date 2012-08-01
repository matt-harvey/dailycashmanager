#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

#include "date.hpp"
#include <jewel/decimal.hpp>
#include <list>
#include <string>

namespace phatbooks
{

class Entry;
class Repeater;


/**
 * Class to represent accounting journals.
 *
 * @todo In API docs, should the accounting logic that Phatbooks
 * is implementing be documented? It should be, but perhaps in a
 * separate file of its own, rather than crammed into the class
 * documentation.
 */
class Journal
{
public:

	/**
	 * This constructor initializes journal date
	 * to null date and journal comment to empty string.
	 * The journal starts out with an empty list of entries
	 * and an empty list of repeaters, and is marked as
	 * non-posted.
	 * 
	 * @param p_is_actual determines whether this will be
	 * an "actuals" journal (as opposed to a budget journal).
	 *
	 * Does not throw.
	 */
	Journal(bool p_is_actual = true);

	/**
	 * Creates an Entry and adds it to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 *
	 * @param account_name name of account of added entry
	 * @param entry_comment Comment ("narration") for entry
	 * @param p_amount Amount of entry
	 */
	void add_entry
	(	std::string const& account_name,
		std::string const& entry_comment = "",
		jewel::Decimal const& p_amount = jewel::Decimal("0")
	);


private:
	bool m_is_actual;
	bool m_is_posted;
	DateType m_date;	
	std::string m_comment;
	std::list<Entry> m_entry_list;
	std::list<Repeater> m_repeater_list;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp

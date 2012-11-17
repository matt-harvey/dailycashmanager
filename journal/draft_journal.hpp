#ifndef GUARD_draft_journal_hpp
#define GUARD_draft_journal_hpp

#include "journal.hpp"
#include "sqloxx/handle.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class Entry;
class PhatbooksDatabaseConnection;
class Repeater;

class DraftJournal:
	public sqloxx::PersistentObject
	<	DraftJournal,
		PhatbooksDatabaseConnection
	>,
	public Journal
{
public:



	/**
	 * Set name of DraftJournal.
	 */
	void set_name(std::string const& p_name);
	
	/**
	 * Add a Repeater to the DraftJournal.
	 */
	void add_repeater(boost::shared_ptr<Repeater> repeater);
	
	/**
	 * @returns name of DraftJournal.
	 */
	std::string name();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(DraftJournal& rhs);


};

}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp

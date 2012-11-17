#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "journal.hpp"
#include "date.hpp"
#include "sqloxx/handle.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace phatbooks
{

class PhatbooksDatabaseConnection;
class Entry;

class OrdinaryJournal:
	public sqloxx::PersistentObject
	<	OrdinaryJournal,
		PhatbooksDatabaseConnection
	>,
	public Journal
{
public:

	typedef
		sqloxx::PersistentObject<OrdinaryJournal, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;




	/**
	 * Create an OrdinaryJournal from a Journal. Note the data members
	 * specific to OrdinaryJournal will be uninitialized. All other
	 * members will be ***shallow-copied*** from p_journal. You must also
	 * pass a shared_ptr to the database connection, as the Journal base
	 * object does not have a database connection associated with it.
	 */
	OrdinaryJournal
	(	Journal const& p_journal,
		boost::shared_ptr<PhatbooksDatabaseConnection> const&
	);

	~OrdinaryJournal();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(OrdinaryJournal& rhs);

private:
	
	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	OrdinaryJournal(OrdinaryJournal const& rhs);

	void do_load();

	void do_save_existing();
	void do_save_new();

};


}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp

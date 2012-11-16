#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "journal.hpp"
#include "date.hpp"
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
	 * Change whether Journal is actual or budget
	 * 
	 * Does not throw.
	 */
	void set_whether_actual(bool p_is_actual);

	/**
	 * Set comment for journal
	 *
	 * Does not throw, except possibly \c std::bad_alloc in extreme
	 * circumstances.
	 */
	void set_comment(std::string const& p_comment);

	/**
	 * Add an Entry to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 */
	void add_entry(boost::shared_ptr<Entry> entry);

	bool is_actual();

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment();

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Implement it! Note, thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced();


	std::vector< boost::shared_ptr<Entry> > const& entries();


	/**
	 * Create the tables required for the persistence of
	 * OrdinaryJournal instances in a SQLite database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" OrdinaryJournal, that will not yet
	 * correspond to any particular object in the database
	 */
	explicit
	OrdinaryJournal
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
	);

	/**
	 * Get an OrdinaryJournal by id from the database.
	 */
	OrdinaryJournal
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection,
		Id p_id
	);

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
	 * Set date of journal.
	 *
	 * Does not throw.
	 */
	void set_date(boost::gregorian::date const& p_date);

	/**
	 * @returns journal date.
	 *
	 * @todo Verify throwing behaviour and determine dependence on DateRep.
	 */
	boost::gregorian::date date();

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

	/* Note this function is not redefined here as we want
	 * it to call Journal::do_get_table_name, which returns
	 * "journal", the name of the table that controls assignment
	 * of the id to all Journal instances, regardless of derived
	 * class.
	 */
	// std::string do_get_table_name() const;

	// Sole non-inherited data member. Note this is of a type where copying
	// does not throw. If we ever add more data members here and/or change
	// this one's type, it MAY be necessary to wrap this with pimpl to
	// to preserve exception-safe laoding via copy-and-swap.
	boost::optional<DateRep> m_date;
};


}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp

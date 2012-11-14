#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "journal.hpp"
#include "date.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>


namespace phatbooks
{

class PhatbooksDatabaseConnection;


class OrdinaryJournal: public Journal
{
public:

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
	 * members will be ***shallow-copied*** from p_journal.
	 */
	OrdinaryJournal(Journal const& p_journal);

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

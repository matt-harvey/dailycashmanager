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
	private Journal
{
public:

	static std::string primary_table_name();

	typedef
		sqloxx::PersistentObject<DraftJournal, PhatbooksDatabaseConnection>
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
	void add_entry(sqloxx::Handle<Entry> entry);


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

	std::vector < sqloxx::Handle<Entry> > const& entries();


	/**
	 * Create the tables required for the persistence
	 * of DraftJournal instances in a SQLite database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" DraftJournal, that will not yet
	 * correspond to any particular object in the database
	 */
	explicit
	DraftJournal
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
	);

	/**
	 * Get a DraftJournal by id from the database.
	 */
	DraftJournal
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection,
		Id p_id
	);

	/**
	 * Create a DraftJournal from a Journal. Note the data members
	 * specific to DraftJournal will be uninitialized. All other
	 * members will be ***shallow-copied*** from p_journal. You must
	 * also pass a shared_ptr to the database connection, as the Journal
	 * base object does not have a database connection associated with it.
	 */
	DraftJournal
	(	Journal const& p_journal,
		boost::shared_ptr<PhatbooksDatabaseConnection> const&
	);

	/**
	 * Destructor.
	 */
	~DraftJournal();


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


private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	DraftJournal(DraftJournal const& rhs);	

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

	struct DraftJournalData
	{
		boost::optional<std::string> name;
		std::vector< boost::shared_ptr<Repeater> > repeaters;
	};

	boost::scoped_ptr<DraftJournalData> m_dj_data;
};

}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp

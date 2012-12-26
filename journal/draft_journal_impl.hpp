#ifndef GUARD_draft_journal_impl_hpp
#define GUARD_draft_journal_impl_hpp

#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/persistent_object.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class Entry;
class Repeater;

class DraftJournalImpl:
	public sqloxx::PersistentObject
	<	DraftJournalImpl,
		PhatbooksDatabaseConnection
	>,
	private Journal
{
public:
	typedef
		sqloxx::PersistentObject<DraftJournalImpl, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;
	typedef
		sqloxx::IdentityMap<DraftJournalImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	static std::string primary_table_name();
	static std::string primary_key_name();

	/**
	 * Create the tables required for the persistence
	 * of DraftJournalImpl instances in a SQLite database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" DraftJournalImpl, that will not yet
	 * correspond to any particular object in the database
	 */
	explicit
	DraftJournalImpl(IdentityMap& p_identity_map);

	/**
	 * Get a DraftJournalImpl by id from the database.
	 */
	DraftJournalImpl
	(	IdentityMap& p_identity_map,
		Id p_id
	);

	/**
	 * Create a DraftJournalImpl from a Journal. Note the data members
	 * specific to DraftJournalImpl will be uninitialized. All other
	 * members will be ***shallow-copied*** from p_journal. You must
	 * also pass a shared_ptr to the database connection, as the Journal
	 * base object does not have a database connection associated with it.
	 */
	DraftJournalImpl
	(	Journal const& p_journal,
		IdentityMap& p_identity_map
	);

	/**
	 * Destructor.
	 */
	~DraftJournalImpl();

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
	void add_entry(Entry& entry);


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

	std::vector<Entry> const& entries();

	/**
	 * Set name of DraftJournalImpl.
	 */
	void set_name(std::string const& p_name);
	
	/**
	 * Add a Repeater to the DraftJournalImpl.
	 */
	void add_repeater(Repeater& repeater);
	
	/**
	 * @returns name of DraftJournalImpl.
	 */
	std::string name();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(DraftJournalImpl& rhs);

	std::string repeater_description();


private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	DraftJournalImpl(DraftJournalImpl const& rhs);	

	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();

	struct DraftJournalData;

	boost::scoped_ptr<DraftJournalData> m_dj_data;
};


struct DraftJournalImpl::DraftJournalData
{
	boost::optional<std::string> name;
	std::vector<Repeater> repeaters;
};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_impl_hpp

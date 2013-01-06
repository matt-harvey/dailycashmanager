#ifndef GUARD_draft_journal_impl_hpp
#define GUARD_draft_journal_impl_hpp

#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
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
	private ProtoJournal
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

	void remove_entry(Entry& entry);

	bool is_actual();

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment();


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

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to DraftJournalImpl.
	 */
	void mimic(ProtoJournal const& rhs);

private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	DraftJournalImpl(DraftJournalImpl const& rhs);	

	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void do_remove();

	void clear_repeaters();

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

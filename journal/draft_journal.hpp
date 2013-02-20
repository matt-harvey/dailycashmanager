#ifndef GUARD_draft_journal_hpp
#define GUARD_draft_journal_hpp

#include "b_string.hpp"
#include "draft_journal_impl.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <ostream>
#include <vector>

namespace phatbooks
{

class Entry;
class PhatbooksDatabaseConnection;
class Repeater;

/**
 * Represents an accounting journal that has not been posted, i.e. has
 * not been reflected in the "economic status" of the accounting entity.
 * A DraftJournal may neverthless be saved in the database.
 *
 * A DraftJournal may serve two purposes. By saving a "half-finished"
 * transaction
 * as a DraftJournal, a user may keep it and return to it in future, to
 * finish preparing the journal and then post it (resulting in an
 * OrdinaryJournal being posted to the database).
 * Alternatively, a DraftJournal may have Repeaters associated with it.
 * A DraftJournal with Repeaters corresponds to a recurring transaction,
 * that is automatically posted by the application at specified intervals.
 *
 * @todo HIGH PRIORITY If the DraftJournal does not balance, we need to
 * avoid posting with autoposts. Furthermore, we need to include wording
 * in the DraftJournal description alerting the user to the fact that it
 * will not be posted for this reason.
 */
class DraftJournal:
	public PersistentJournal,
	virtual private PhatbooksPersistentObject<DraftJournalImpl>
{
public:
	typedef
		PhatbooksPersistentObject<DraftJournalImpl>
		PhatbooksPersistentObject;
	typedef PhatbooksPersistentObjectBase::Id Id;

	/**
	 * Create the tables required for the persistence of DraftJournal
	 * instances to the database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" DraftJournal, that will not yet correspond to
	 * any particular object in the database.
	 */
	explicit DraftJournal
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Get a DraftJournal by id from the database. Throws if there
	 * is not DraftJournal with this id.
	 */
	DraftJournal
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * @returns a DraftJournal by id from the datbase. Does not check
	 * whether there actually is a DraftJournal with this id. As a result
	 * this is faster than calling the (normal) constructor; but the caller
	 * should be sure that a DraftJournal with this id exists, before calling
	 * this fuction.
	 */
	static DraftJournal create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	void set_name(BString const& p_name);
	void add_repeater(Repeater& repeater);
	BString name() const;

	/**
	 * @returns a verbal description of the automatic postings associated
	 * with the DraftJournal (or an empty string if there are none).
	 *
	 * @todo Test.
	 */
	BString repeater_description() const;

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to DraftJournal.
	 */
	void mimic(ProtoJournal const& rhs);

	bool has_repeaters() const;

	void clear_repeaters();


private:

	// Define pure virtual functions inherited from Journal
	std::vector<Entry> const& do_get_entries() const;
	void do_set_whether_actual(bool p_is_actual);
	void do_set_comment(BString const& p_comment);
	void do_add_entry(Entry& entry);
	void do_remove_entry(Entry& entry);
	BString do_get_comment() const;
	bool do_get_whether_actual() const;
	
	// Redefine impure virtual function inherited from Journal
	void do_output(std::ostream& os) const;

	DraftJournal(sqloxx::Handle<DraftJournalImpl> const& p_handle);

};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp

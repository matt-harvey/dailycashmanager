// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_hpp
#define GUARD_draft_journal_hpp

#include "b_string.hpp"
#include "draft_journal_impl.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
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
 * Note attempting to call remove() on a DraftJournal that is equal to
 * the database_connection().budget_instrument() will result in
 * PreservedRecordDeletionException being thrown. (remove() does not
 * appear in this header at it is inherited from
 * PhatbooksPersistentObject<DraftJournalImpl>, and is implemented
 * in DraftJournalImpl.)
 */
class DraftJournal:
	public PersistentJournal,
	virtual public PhatbooksPersistentObject<DraftJournalImpl>
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

	/**
	 * @returns \c true if and only if \c p_name is the name of a
	 * DraftJournalImpl stored in the database.
	 *
	 * @todo Document throwing behaviour.
	 */
	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_name
	);

	/**
	 * @returns \e true if the only DraftJournal
	 * that is saved is p_database_connection.budget_instrument(), or
	 * if there are no DraftJournals saved at all. If there is one
	 * or more DraftJournal saved other than
	 * p_database_connection.budget_instrument(), then this
	 * function returns false.
	 */
	static bool no_user_draft_journals_saved
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	void set_name(BString const& p_name);
	void push_repeater(Repeater& repeater);
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
	void mimic(DraftJournal const& rhs);

	std::vector<Repeater> const& repeaters() const;

	bool has_repeaters() const;

	void clear_repeaters();

	
private:

	// Define pure virtual functions inherited from Journal
	std::vector<Entry> const& do_get_entries() const;
	void do_set_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);
	void do_set_comment(BString const& p_comment);
	void do_set_fulcrum(size_t p_fulcrum);
	void do_push_entry(Entry& entry);
	void do_remove_entry(Entry& entry);
	void do_clear_entries();
	BString do_get_comment() const;
	transaction_type::TransactionType do_get_transaction_type() const;
	size_t do_get_fulcrum() const;
	
	// Redefine impure virtual function inherited from Journal
	void do_output(std::ostream& os) const;

	DraftJournal(sqloxx::Handle<DraftJournalImpl> const& p_handle);

};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp

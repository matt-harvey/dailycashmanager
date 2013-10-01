// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_impl_hpp_8602723767330276
#define GUARD_draft_journal_impl_hpp_8602723767330276

#include "persistent_journal.hpp"
#include <sqloxx/persistence_traits.hpp>

namespace phatbooks
{

class DraftJournalImpl;

}  // namespace phatbooks



namespace sqloxx
{

template <>
struct PersistenceTraits<phatbooks::DraftJournalImpl>
{
	typedef phatbooks::PersistentJournal PrimaryT;
};

}  // namespace sqloxx



#include "entry_handle_fwd.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "repeater_handle_fwd.hpp"
#include "transaction_type.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/persistent_object.hpp>
#include <wx/string.h>
#include <memory>
#include <string>
#include <vector>

namespace phatbooks
{

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
 */
class DraftJournalImpl: virtual public PersistentJournal
{
public:

	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

	/**
	 * Create the tables required for the persistence of DraftJournal
	 * instances to the database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

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

	/**
	 * Construct a "raw" DraftJournal, not yet saved in the database,
	 * and without an Id.
	 *
	 * The Signature parameter prevents this constructor from being
	 * called except by IdentityMap.
	 *
	 * Ordinary client code should use DraftJournalHandle, not DraftJournal.
	 */
	DraftJournalImpl
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Get a DraftJournal by Id from the database.
	 *
	 * The Signature parameter prevents this constructor from being
	 * called except by IdentityMap.
	 *
	 * Ordinary client code should use DraftJournalHandle, not DraftJournal.
	 */
	DraftJournalImpl
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// copy constructor is private

	DraftJournalImpl(DraftJournalImpl&&) = delete;
	DraftJournalImpl& operator=(DraftJournalImpl const&) = delete;
	DraftJournalImpl& operator=(DraftJournalImpl&&) = delete;

	~DraftJournalImpl();

	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_name
	);
	
	/**
	 * @todo Document.
	 */
	void set_transaction_type
	(	TransactionType p_transaction_type
	);

	/**
	 * Set comment for journal
	 *
	 * Does not throw, except possibly \c std::bad_alloc in extreme
	 * circumstances.
	 */
	void set_comment(wxString const& p_comment);

	/**
	 * Add an Entry to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 */
	void push_entry(EntryHandle const& entry);

	void remove_entry(EntryHandle const& entry);

	TransactionType transaction_type();

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	wxString comment();

	std::vector<EntryHandle> const& entries();

	void set_name(wxString const& p_name);
	
	void push_repeater(RepeaterHandle const& repeater);
	
	wxString name();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(DraftJournalImpl& rhs);

	wxString repeater_description();

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to DraftJournalImpl.
	 */
	void mimic(Journal& rhs);
	void mimic(DraftJournalImpl& rhs);  // not const& rhs because loading required

	std::vector<RepeaterHandle> const& repeaters();

	bool has_repeaters();

	void clear_repeaters();
	void clear_entries();

private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	DraftJournalImpl(DraftJournalImpl const& rhs);	

	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();

	/**
	 * Note if the DraftJournalImpl corresponds to the budget instrument
	 * for database_connection(), then PreservedRecordDeletionException is
	 * thrown.
	 */
	void do_remove();

	struct DraftJournalData;

	std::unique_ptr<DraftJournalData> m_dj_data;
};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_impl_hpp_8602723767330276

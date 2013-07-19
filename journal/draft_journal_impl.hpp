// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_impl_hpp
#define GUARD_draft_journal_impl_hpp

#include "b_string.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <sqloxx/persistent_object.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class Entry;
class Repeater;

/**
 * Implementation class for DraftJournal. Note multiple DraftJournal instances
 * may share the same underlying DraftJournalImpl instance.
 */
class DraftJournalImpl:
	public sqloxx::PersistentObject
	<	DraftJournalImpl,
		PhatbooksDatabaseConnection
	>,
	private ProtoJournal
{
public:
	typedef
		sqloxx::PersistentObject
		<	DraftJournalImpl,
			PhatbooksDatabaseConnection
		>
		PersistentObject;

	typedef typename PersistentObject::Id Id;

	typedef
		sqloxx::IdentityMap<DraftJournalImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit
	DraftJournalImpl(IdentityMap& p_identity_map);

	DraftJournalImpl
	(	IdentityMap& p_identity_map,
		Id p_id
	);

	~DraftJournalImpl();

	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_name
	);

	static bool no_user_draft_journals_saved
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	
	/**
	 * @todo Document.
	 */
	void set_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);

	/**
	 * Set comment for journal
	 *
	 * Does not throw, except possibly \c std::bad_alloc in extreme
	 * circumstances.
	 */
	void set_comment(BString const& p_comment);

	/**
	 * Set "fulcrum" for Journal. The fulcrum is the point at which the
	 * notional "source" Entries end and the "destination" Entries
	 * begin. This is important for display purposes in the UI.
	 * If there is exactly 1 source Entry, then the fulcrum will be 1.
	 */
	void set_fulcrum(size_t p_fulcrum);

	/**
	 * Add an Entry to the Journal.
	 *
	 * @todo Figure out throwing behaviour. Should it check that
	 * the account exists? Etc. Etc.
	 */
	void push_entry(Entry& entry);

	void remove_entry(Entry& entry);

	size_t fulcrum();

	transaction_type::TransactionType transaction_type();

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	BString comment();

	std::vector<Entry> const& entries();

	void set_name(BString const& p_name);
	
	void push_repeater(Repeater& repeater);
	
	BString name();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(DraftJournalImpl& rhs);

	BString repeater_description();

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to DraftJournalImpl.
	 */
	void mimic(ProtoJournal const& rhs);
	void mimic(DraftJournalImpl& rhs);  // not const& rhs because loading required

	std::vector<Repeater> const& repeaters();

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

	boost::scoped_ptr<DraftJournalData> m_dj_data;
};


struct DraftJournalImpl::DraftJournalData
{
	boost::optional<BString> name;
	std::vector<Repeater> repeaters;
};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_impl_hpp

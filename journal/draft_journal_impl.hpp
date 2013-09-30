// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_draft_journal_impl_hpp_8602723767330276
#define GUARD_draft_journal_impl_hpp_8602723767330276

#include "entry_handle_fwd.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "repeater_handle.hpp"
#include "transaction_type.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/persistent_object.hpp>
#include <boost/optional.hpp>
#include <wx/string.h>
#include <memory>
#include <string>
#include <vector>

namespace phatbooks
{

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

	typedef
		sqloxx::IdentityMap<DraftJournalImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	DraftJournalImpl
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	DraftJournalImpl
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// copy constructor is private

	DraftJournalImpl(DraftJournalImpl&&) = delete;
	DraftJournalImpl& operator=(DraftJournalImpl const&) = delete;
	DraftJournalImpl& operator=(DraftJournalImpl&&) = delete;

	~DraftJournalImpl() = default;

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
	void mimic(ProtoJournal const& rhs);
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


struct DraftJournalImpl::DraftJournalData
{
	boost::optional<wxString> name;
	std::vector<RepeaterHandle> repeaters;
};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_impl_hpp_8602723767330276

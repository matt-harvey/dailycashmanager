// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_persistent_journal_hpp_46241805630848654
#define GUARD_persistent_journal_hpp_46241805630848654

#include "entry_handle.hpp"
#include "journal.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/persistent_object.hpp>
#include <ostream>

namespace phatbooks
{

/**
 * Common abstract base class for subclasses of Journal that
 * can be persisted to a database.
 */
class PersistentJournal:
	public sqloxx::PersistentObject
	<	PersistentJournal,
		PhatbooksDatabaseConnection
	>,
	public Journal
{
public:

	typedef
		sqloxx::PersistentObject
		<	PersistentJournal,
			PhatbooksDatabaseConnection
		>
		PersistentObject;
	
	typedef
		sqloxx::IdentityMap<PersistentJournal, PhatbooksDatabaseConnection>
		IdentityMap;
	
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	static std::string primary_table_name();
	static std::string primary_key_name();

	PersistentJournal
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	PersistentJournal
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);
		
	// copy constructor is protected
	
	PersistentJournal(PersistentJournal&&) = delete;
	PersistentJournal& operator=(PersistentJournal const&) = delete;
	PersistentJournal& operator=(PersistentJournal&&) = delete;
	virtual ~PersistentJournal();

protected:

	PersistentJournal(PersistentJournal const& rhs);

	virtual void swap(PersistentJournal& rhs);

	void do_load_journal_core();
	void do_save_existing_journal_core();
	sqloxx::Id do_save_new_journal_core();
	void do_ghostify_journal_core();


	// virtual void do_save_existing() override = 0;
	// virtual void do_save_new() override;
	// virtual void do_ghostify() override;
	// virtual void do_remove() override;

	std::vector<EntryHandle> const& do_get_entries() override;
	void do_set_transaction_type(TransactionType p_transaction_type) override;
	void do_set_comment(wxString const& p_comment) override;
	void do_push_entry(EntryHandle const& p_entry) override;
	void do_remove_entry(EntryHandle const& p_entry) override;
	void do_clear_entries() override;
	wxString do_get_comment() override;
	TransactionType do_get_transaction_type();
	
};


bool
has_entry_with_id(PersistentJournal& journal, sqloxx::Id entry_id);

sqloxx::Id
max_journal_id(PhatbooksDatabaseConnection& dbc);

sqloxx::Id
min_journal_id(PhatbooksDatabaseConnection& dbc);

/**
 * Journal id must exist.
 */
bool
journal_id_is_draft(PhatbooksDatabaseConnection& dbc, sqloxx::Id);






}  // namespace phatbooks


#endif  // GUARD_persistent_journal_hpp_46241805630848654

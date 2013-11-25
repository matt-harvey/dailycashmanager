/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_persistent_journal_hpp_46241805630848654
#define GUARD_persistent_journal_hpp_46241805630848654

#include "journal.hpp"
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/persistent_object.hpp>
#include <ostream>

namespace dcm
{

// begin forward declarations

class Entry;

// end forward declarations

/**
 * Common abstract base class for subclasses of Journal that
 * can be persisted to a database.
 *
 * NOTE, if an attempt is made to save a PersistentJournal with
 * TransactionType::envelope (i.e. a budget journal) where at least one
 * of the Entries in the PersistentJournal has an Account with
 * account_super_type() that is not of AccountSuperType::pl, then,
 * InvalidJournalException will be thrown. This is because we do not
 * want any (non-zero) balance sheet budget amounts in the database.
 */
class PersistentJournal:
	public sqloxx::PersistentObject
	<	PersistentJournal,
		DcmDatabaseConnection
	>,
	public Journal
{
public:

	typedef
		sqloxx::PersistentObject
		<	PersistentJournal,
			DcmDatabaseConnection
		>
		PersistentObject;
	
	typedef
		sqloxx::IdentityMap<PersistentJournal> IdentityMap;
	
	static void setup_tables(DcmDatabaseConnection& dbc);

	static std::string exclusive_table_name();
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

	void load_journal_core();
	void save_existing_journal_core();
	sqloxx::Id save_new_journal_core();
	void ghostify_journal_core();

	std::vector<sqloxx::Handle<Entry> > const& do_get_entries() override;
	void do_set_transaction_type(TransactionType p_transaction_type) override;
	void do_set_comment(wxString const& p_comment) override;
	void do_push_entry(sqloxx::Handle<Entry> const& p_entry) override;
	void do_remove_entry(sqloxx::Handle<Entry> const& p_entry) override;
	void do_clear_entries() override;
	wxString do_get_comment() override;
	TransactionType do_get_transaction_type() override;

private:

	/**
	 * @throws InvalidJournalException if and only if the PersistentJournal
	 * is a budget Journal that contains an Entry with a balance sheet
	 * Account.
	 */
	void ensure_pl_only_budget();

	/**
	 * @returns true if and only if posting the Journal would cause arithmetic
	 * overflow in Account balances.
	 */
	bool would_cause_overflow();
};


bool
has_entry_with_id(PersistentJournal& journal, sqloxx::Id entry_id);

sqloxx::Id
max_journal_id(DcmDatabaseConnection& dbc);

sqloxx::Id
min_journal_id(DcmDatabaseConnection& dbc);

/**
 * Journal id must exist.
 */
bool
journal_id_is_draft(DcmDatabaseConnection& dbc, sqloxx::Id);






}  // namespace dcm


#endif  // GUARD_persistent_journal_hpp_46241805630848654

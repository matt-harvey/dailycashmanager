/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_persistent_journal_hpp_46241805630848654
#define GUARD_persistent_journal_hpp_46241805630848654

#include "journal.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/persistent_object.hpp>
#include <ostream>

namespace phatbooks
{

// begin forward declarations

class Entry;

// end forward declarations

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
		sqloxx::IdentityMap<PersistentJournal> IdentityMap;
	
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

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

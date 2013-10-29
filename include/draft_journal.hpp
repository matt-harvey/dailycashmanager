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


#ifndef GUARD_draft_journal_hpp_8602723767330276
#define GUARD_draft_journal_hpp_8602723767330276

#include "persistent_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include "transaction_type.hpp"
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/persistence_traits.hpp>
#include <sqloxx/persistent_object.hpp>
#include <wx/string.h>
#include <memory>
#include <string>
#include <vector>

namespace phatbooks
{

class DraftJournal;
class Repeater;

}  // namespace phatbooks

namespace sqloxx
{

template <>
struct PersistenceTraits<phatbooks::DraftJournal>
{
	typedef phatbooks::PersistentJournal Base;
};

}  // namespace sqloxx


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
class DraftJournal: public PersistentJournal
{
public:

	static std::string exclusive_table_name();

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
	 * Ordinary client code should use sqloxx::Handle<DraftJournal>, not
	 * DraftJournal directly.
	 */
	DraftJournal
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Get a DraftJournal by Id from the database.
	 *
	 * The Signature parameter prevents this constructor from being
	 * called except by IdentityMap.
	 *
	 * Ordinary client code should use sqloxx::Handle<DraftJournal>, not
	 * DraftJournal directly.
	 */
	DraftJournal
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// copy constructor is private

	DraftJournal(DraftJournal&&) = delete;
	DraftJournal& operator=(DraftJournal const&) = delete;
	DraftJournal& operator=(DraftJournal&&) = delete;

	~DraftJournal();

	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_name
	);

	void set_name(wxString const& p_name);
	
	void push_repeater(sqloxx::Handle<Repeater> const& repeater);
	
	wxString name();

	wxString repeater_description();

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to DraftJournal.
	 */
	void mimic(Journal& rhs);
	void mimic(DraftJournal& rhs);  // not const& rhs because loading required

	std::vector<sqloxx::Handle<Repeater> > const& repeaters();

	bool has_repeaters();

	void clear_repeaters();

private:

	void swap(DraftJournal& rhs);

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	DraftJournal(DraftJournal const& rhs);	

	void do_load() override;
	void do_save_existing() override;
	void do_save_new() override;
	void do_ghostify() override;

	/**
	 * Note if the DraftJournal corresponds to the budget instrument
	 * for database_connection(), then PreservedRecordDeletionException is
	 * thrown.
	 */
	void do_remove() override;

	struct DraftJournalData;

	std::unique_ptr<DraftJournalData> m_dj_data;
};


}  // namespace phatbooks


#endif  // GUARD_draft_journal_hpp_8602723767330276

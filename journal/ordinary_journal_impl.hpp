#ifndef GUARD_ordinary_journal_impl_hpp
#define GUARD_ordinary_journal_impl_hpp

#include "journal.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace phatbooks
{

class DraftJournal;

class OrdinaryJournalImpl:
	public sqloxx::PersistentObject
	<	OrdinaryJournalImpl,
		PhatbooksDatabaseConnection
	>,
	private Journal
{
public:

	typedef
		sqloxx::PersistentObject
		<	OrdinaryJournalImpl,
			PhatbooksDatabaseConnection
		>
		PersistentObject;

	typedef sqloxx::IdentityMap
		<	OrdinaryJournalImpl,
			PhatbooksDatabaseConnection
		>
		IdentityMap;

	typedef sqloxx::Id Id;

	static std::string primary_table_name();
	static std::string primary_key_name();

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
	 * @todo Figure out throwing behaviour.
	 */
	void add_entry(Entry& entry);

	bool is_actual();

	/**
	 * @returns journal comment.
	 *
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment();

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Implement it! Note, thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced();

	std::vector<Entry> const& entries();

	/**
	 * Create the tables required for the persistence of
	 * OrdinaryJournalImpl instances in a SQLite database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" OrdinaryJournalImpl, that will not yet
	 * correspond to any particular object in the database
	 */
	explicit
	OrdinaryJournalImpl
	(	IdentityMap& p_identity_map	
	);

	/**
	 * Get an OrdinaryJournalImpl by id from the database.
	 */
	OrdinaryJournalImpl
	(	IdentityMap& p_identity_map,	
		Id p_id
	);

	/**
	 * Create an OrdinaryJournalImpl from a Journal. Note the data members
	 * specific to OrdinaryJournalImpl will be uninitialized. All other
	 * members will be ***shallow-copied*** from p_journal. You must also
	 * pass a shared_ptr to the database connection, as the Journal base
	 * object does not have a database connection associated with it.
	 */
	OrdinaryJournalImpl
	(	Journal const& p_journal,
		IdentityMap& p_identity_map
	);

	~OrdinaryJournalImpl();

	/**
	 * Set date of journal.
	 *
	 * Does not throw.
	 */
	void set_date(boost::gregorian::date const& p_date);

	/**
	 * @returns journal date.
	 *
	 * @todo Verify throwing behaviour and determine dependence on DateRep.
	 */
	boost::gregorian::date date();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(OrdinaryJournalImpl& rhs);

	/**
	 * Take on the attributes from \e rhs, where these exist and are
	 * applicable to OrdinaryJournalImpl; but do \e not take on the \e id
	 * attribute of \e rhs.
	 *
	 * TODO I seem to be stuck with the Journal& one being const, for
	 * perverse reasons. Can this be fixed?
	 */
	void mimic(Journal& rhs);
	void mimic(DraftJournal const& rhs);
	void mimic(OrdinaryJournalImpl& rhs);

	void clear_entries();

private:

	/**
	 * Where J is Journal, DraftJournalImpl or OrdinaryJournalImpl,
	 * cause *this to take on the attributes of rhs that would be common
	 * to all three kinds of
	 * journal. Thus, for example, where J is OrdinaryJournal, *this does
	 * \e not take on the \e date attribute of rhs, since Journal and
	 * DraftJournal do not have a \e date attribute.
	 * Note however that the \e id attribute is \e never taken from the
	 * rhs.
	 * This does \e not offer the strong guarantee by itself.
	 */
	template <typename J>
	void mimic_core(J& rhs);

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	OrdinaryJournalImpl(OrdinaryJournalImpl const& rhs);


	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();


	// Sole non-inherited data member. Note this is of a type where copying
	// does not throw. If we ever add more data members here and/or change
	// this one's type, it MAY be necessary to wrap this with pimpl to
	// to preserve exception-safe laoding via copy-and-swap.
	boost::optional<DateRep> m_date;
};


template <typename J>
void
OrdinaryJournalImpl::mimic_core(J& rhs)
{
	set_whether_actual(rhs.is_actual());
	set_comment(rhs.comment());
	clear_entries();
	if (!rhs.entries().empty())
	{
		for 
		(	std::vector<Entry>::const_iterator it = rhs.entries().begin(),
				end = rhs.entries().end();
			it != end;
			++it
		)
		{
			Entry entry(database_connection());
			entry.mimic(*it);
			if (has_id())
			{
				entry.set_journal_id(id());
			}
			Journal::add_entry(entry);
		}
	}
	return;
}


			
				

}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_impl_hpp

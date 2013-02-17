#ifndef GUARD_ordinary_journal_impl_hpp
#define GUARD_ordinary_journal_impl_hpp

#include "date.hpp"
#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <wx/string.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class DraftJournal;

class OrdinaryJournalImpl:
	public sqloxx::PersistentObject
	<	OrdinaryJournalImpl,
		PhatbooksDatabaseConnection
	>,
	private ProtoJournal
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
	 * Does not throw.
	 */
	void set_whether_actual(bool p_is_actual);

	/**
	 * Does not throw, except possibly \c std::bad_alloc in extreme
	 * circumstances.
	 */
	void set_comment(wxString const& p_comment);

	/**
	 * @todo Figure out throwing behaviour.
	 */
	void add_entry(Entry& entry);

	void remove_entry(Entry& entry);

	bool is_actual();

	/**
	 * Does not throw, except perhaps \c std::bad_alloc in
	 * extreme circumstances.
	 */
	wxString comment();

	std::vector<Entry> const& entries();

	/**
	 * Create the tables required for the persistence of
	 * OrdinaryJournalImpl instances in a SQLite database.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Construct a "raw" OrdinaryJournalImpl, that will not yet
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

	~OrdinaryJournalImpl();

	/**
	 * Does not throw.
	 */
	void set_date(boost::gregorian::date const& p_date);

	/**
	 * @todo Verify throwing behaviour and determine dependence on DateRep.
	 */
	boost::gregorian::date date();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(OrdinaryJournalImpl& rhs);

	/**
	 * Take on the attributes \e rhs, where these exist and are
	 * applicable to OrdinaryJournalImpl; but do \e not take on the \e id
	 * attribute of \e rhs, or the date.
	 */
	void mimic(Journal const& rhs);

private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	OrdinaryJournalImpl(OrdinaryJournalImpl const& rhs);


	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void do_remove();


	// Sole non-inherited data member. Note this is of a type where copying
	// does not throw. If we ever add more data members here and/or change
	// this one's type, it MAY be necessary to wrap this with pimpl to
	// to preserve exception-safe laoding via copy-and-swap.
	boost::optional<DateRep> m_date;
};


			
				

}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_impl_hpp

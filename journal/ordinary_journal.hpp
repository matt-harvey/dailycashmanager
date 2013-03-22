#ifndef GUARD_ordinary_journal_hpp
#define GUARD_ordinary_journal_hpp

#include "b_string.hpp"
#include "entry.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "ordinary_journal_impl.hpp"
#include "proto_journal.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream>
#include <ostream>
#include <vector>

namespace phatbooks
{

class DraftJournal;
class PhatbooksDatabaseConnection;


/**
 * Represents an accounting journal that is, or will be, posted and
 * thereby reflected as a change in the economic state of the accounting
 * entity. The posting occurs when the \e save() method is called on the
 * OrdinaryJournal.
 */
class OrdinaryJournal:
	public PersistentJournal,
	virtual public PhatbooksPersistentObject<OrdinaryJournalImpl>
{
public:

	typedef
		PhatbooksPersistentObject<OrdinaryJournalImpl>
		PhatbooksPersistentObject;
	
	typedef PhatbooksPersistentObject::Id Id;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Construct a "raw" OrdinaryJournal, that will not yet correspond to
	 * any particular object in the database.
	 */
	explicit
	OrdinaryJournal
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Retrieve an OrdinaryJournal from the database by id.
	 * Throws if there is no OrdinaryJournal with this id.
	 */
	OrdinaryJournal
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * @returns an OrdinaryJournal theoretically corresponding to one that
	 * has been saved to the database with id \e p_id. However, this
	 * function does not check whether there actually is an OrdinaryJournal
	 * with this id in the database. It is the caller's responsibility to
	 * be sure there is such an OrdinaryJournal, before calling this function.
	 * This function is a faster way to get an instance of OrdinaryJournal,
	 * than by calling the (normal) constructor that takes an id.
	 */
	static OrdinaryJournal create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection, 
		Id p_id
	);

	void set_date(boost::gregorian::date const& p_date);

	/**
	 * @returns posting date.
	 */
	boost::gregorian::date date() const;

	/**
	 * Take on the attributes of \e rhs, where these exist and are
	 * applicable to OrdinaryJournal; but do \e not take on the \e id of
	 * rhs (and ignore \e next_date() in the case of DraftJournal).
	 */
	void mimic(ProtoJournal const& rhs);
	void mimic(DraftJournal const& rhs);


private:

	// Define pure virtual functions inherited from Journal
	void do_set_whether_actual(bool p_is_actual);
	void do_set_comment(BString const& p_comment);
	void do_push_entry(Entry& entry);
	void do_remove_entry(Entry& entry);
	void do_clear_entries();
	bool do_get_whether_actual() const;
	BString do_get_comment() const;
	std::vector<Entry> const& do_get_entries() const;

	// Virtual function inherited from Journal
	void do_output(std::ostream& os) const;

	OrdinaryJournal(sqloxx::Handle<OrdinaryJournalImpl> const& p_handle);
};


}  // namespace phatbooks


#endif  // GUARD_ordinary_journal_hpp

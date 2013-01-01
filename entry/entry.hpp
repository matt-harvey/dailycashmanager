#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

#include "entry_impl.hpp"
#include "finformat.hpp"
#include "phatbooks_persistent_object.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

class Account;
class PhatbooksDatabaseConnection;

class Entry:
	virtual public PhatbooksPersistentObjectBase,
	private PhatbooksPersistentObjectDetail<EntryImpl>
{
public:
	
	typedef
		PhatbooksPersistentObjectDetail<EntryImpl>
		PhatbooksPersistentObjectDetail;
	
	typedef
		PhatbooksPersistentObjectBase::Id Id;


	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	explicit Entry
	(	PhatbooksDatabaseConnection& p_database_connection
	);
	Entry
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);
	static Entry create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);
	
	/**
	 * Sets the journal_id for the EntryImpl. Note this should NOT
	 * normally be called. The usual way to associate an EntryImpl with a
	 * Journal (or DraftJournal or OrdinaryJournal) is for the EntryImpl
	 * to be added to the Journal via its add_entry(...) method.
	 */
	void set_journal_id(sqloxx::Id p_journal_id);

	void set_account(Account const& p_account);
	void set_comment(std::string const& p_comment);
	void set_amount(jewel::Decimal const& p_amount);
	void set_whether_reconciled(bool p_is_reconciled);

	std::string comment() const;
	jewel::Decimal amount() const;
	Account account() const;
	bool is_reconciled() const;
	
	// WARNING Quick hack
	template <typename JournalType>
	JournalType journal() const
	{
		return impl().journal<JournalType>();
	}

	/**
	 * @returns the posting date of the Entry, assuming it is associated
	 * with an OrdinaryJournal. If it is associated with another kind of
	 * Journal, then behaviour is undefined.
	 *
	 * Note this function is a bit slow.
	 */
	boost::gregorian::date date() const;

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * \e id,\n
	 * \e database_connection, or\n
	 * \e journal_id.
	 */
	void mimic(Entry const& rhs);

private:
	Entry(sqloxx::Handle<EntryImpl> const& p_handle);
};


boost::shared_ptr<std::vector<std::string> >
make_entry_row(Entry const& entry);

boost::shared_ptr<std::vector<std::string> >
make_reversed_entry_row(Entry const& entry);

// Like above but should only be used with ordinary entries (i.e.
// entries of which the Journal is an OrdinaryJournal.
// Includes journal id and date in the entry row produced.
// If the OrdinaryJournal for the Entry does not have an id then
// this will fail.
boost::shared_ptr<std::vector<std::string> >
make_augmented_ordinary_entry_row(Entry const& entry);

}  // namespace phatbooks


#endif  // GUARD_entry_hpp

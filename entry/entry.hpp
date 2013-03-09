#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

#include "b_string.hpp"
#include "entry_impl.hpp"
#include "finformat.hpp"
#include "phatbooks_persistent_object.hpp"
#include <consolixx/column.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

class Account;
class PhatbooksDatabaseConnection;




/**
 * Class representing an accounting entry, i.e. a single line in a journal,
 * i.e. a single change in the balance of an account.
 */
class Entry:
	public PhatbooksPersistentObject<EntryImpl>
{
public:
	
	typedef
		PhatbooksPersistentObject<EntryImpl>
		PhatbooksPersistentObject;
	
	typedef
		PhatbooksPersistentObjectBase::Id Id;

	/*
	 * Set up tables in the database required for the persistence of
	 * EntryImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "raw" Entry, that will not yet correspond to any
	 * particular object in the database, and will not yet be associated
	 * with any particular Journal.
	 */
	explicit Entry
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Get an Entry by id from the database. Throws if there is no
	 * Entry with this id.
	 */
	Entry
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * @returns the Entry identified by \e id in the database; but does
	 * not check whether there is actually an Entry with this id. Thus
	 * it is faster than the checked constructor, but should only be used
	 * if you already know there is an Entry with the given id.
	 */
	static Entry create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);
	
	/**
	 * Sets the journal_id for the EntryImpl. Note this should \e not
	 * normally be called. The usual way to associate an EntryImpl with a
	 * Journal (or DraftJournal or OrdinaryJournal) is for the EntryImpl
	 * to be added to the Journal via its push_entry(...) method.
	 */
	void set_journal_id(Id p_journal_id);

	/**
	 * Set the Account with which this Entry is associated.
	 */
	void set_account(Account const& p_account);
	void set_comment(BString const& p_comment);
	
	/**
	 * Set the amount of the EntryImpl. The amount should be: a positive number
	 * for an actual debit; a negative number for an actual credit; a
	 * negative number for a budget entry that increases the available
	 * funds in an expenditure envelope; and a positive number for a budget
	 * entry that decreases the available funds in an expenditure envelope.
	 *
	 * @todo Should this method perform rounding to the number
	 * of decimal places required for the precision of the Account's
	 * native Commodity?
	 */
	void set_amount(jewel::Decimal const& p_amount);

	/**
	 * Set whether the Entry has been reconciled (e.g. for reconciling to
	 * a bank or credit card statement.
	 */
	void set_whether_reconciled(bool p_is_reconciled);

	/**
	 * @returns Entry comment, i.e. descriptive text generally entered by
	 * the user.
	 */
	BString comment() const;

	/**
	 * @returns amount of Entry (+ve for debits, -ve for credits).
	 * Note that, when the Account of the Entry is a P&L account,
	 * if we consider this P&L account from the point of view of a
	 * budgeting envelope, the \e amount() will be -ve if the Entry
	 * \e increases the
	 * available funds in the envelope,
	 * and +ve if the Entry \e decreases the available funds in an
	 * envelope.
	 */
	jewel::Decimal amount() const;

	/**
	 * @returns the Account that the Entry affects.
	 */
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





	/***************************************************************/
	// Here follow functions for
	// creating consolixx::Columns, to facilitate
	// construction of consolixx::Tables, for displaying
	// Entry-related data in text form.
	// Client is responsible for deleting the heap-allocated object
	// pointed to by the returned pointer.
	// WARNING We use sqloxx::Id here in lieu of OrdinaryJournal::Id, to avoid
	// circular includes. This is a bit imperfect. We have a static
	// assertion in the source file to break compilation if these are
	// ever two different types.
	
	/**
	 * Creates a column that shows the journal id of each entry's journal.
	 * Should not be used unless we KNOW that all the entry's in the
	 * desired range are ordinary (not draft or proto) entries.
	 */
	static consolixx::PlainColumn<Entry, sqloxx::Id>*
		create_ordinary_journal_id_column();

	/**
	 * Creates a column that shows the date of each entry. Should not be
	 * used unless we KNOW that all the entries in range are
	 * ordinary (not draft or proto) entries.
	 */
	static consolixx::PlainColumn<Entry, boost::gregorian::date>*
		create_ordinary_journal_date_column();
	
	/**
	 * Creates a column that shows the id of each entry.
	 */
	static consolixx::PlainColumn<Entry, boost::optional<Id> >*
		create_id_column();
	
	/**
	 * Creates a column that shows the name of the account of
	 * each entry.
	 */
	static consolixx::PlainColumn<Entry, BString>*
		create_account_name_column();

	/**
	 * Creates a column that shows the comment of each entry.
	 */
	static consolixx::PlainColumn<Entry, BString>*
		create_comment_column();
	
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		static consolixx::PlainColumn<Entry, BString>*
			create_commodity_abbreviation_column();
#	endif	

	/**
	 * Creates a column that shows the amount of each entry.
	 */
	static consolixx::PlainColumn<Entry, jewel::Decimal>*
		create_amount_column();

	/**
	 * Creates a column that show the amount of each entry,
	 * with the sign switched.
	 */
	static consolixx::PlainColumn<Entry, jewel::Decimal>*
		create_reversed_amount_column();

	/**
	 * Creates a column that displays each entry's amount and
	 * calculates a total to
	 * show at the foot of the column.
	 */
	static consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
		create_accumulating_amount_column
		(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
		);

	/**
	 * Creates a column that displays each entry's reversed amount and
	 * calculates a total
	 * to show at the foot of the column.
	 */
	static consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
		create_accumulating_reversed_amount_column
		(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
		);

	/**
	 * Creates a column that displays a running total amount, but
	 * does not show a footer.
	 */
	static consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
		create_running_total_amount_column
		(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
		);

	/**
	 * Creates a column that displays a running total of reconciled
	 * amount, but does not show a footer.
	 */
	static consolixx::AccumulatingColumn<Entry, jewel::Decimal>*
		create_running_total_reconciled_amount_column
		(	jewel::Decimal const& p_seed = jewel::Decimal(0, 0)
		);

	/**
	 * Creates a column that shows the reconciliation status
	 * ("y" or "n").
	 */
	static consolixx::PlainColumn<Entry, bool>*
		create_reconciliation_status_column();
	
	// End of section containing functions for creating
	// consolixx::Columns.
	/**************************************************************/



private:
	Entry(sqloxx::Handle<EntryImpl> const& p_handle);
};


// Keep these as std::string, to work with consolixx::Table.

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

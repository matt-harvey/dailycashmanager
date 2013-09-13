// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_hpp_3389501093602507
#define GUARD_entry_hpp_3389501093602507

#include "entry_impl.hpp"
#include "finformat.hpp"
#include "phatbooks_persistent_object.hpp"
#include "transaction_side.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <wx/string.h>
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
	 * Entry objects.
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
	void set_comment(wxString const& p_comment);
	
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
	 * Set the TransactionSide for the Entry (see documentation for
	 * transaction_side::TransactionSide).
	 */
	void set_transaction_side
	(	transaction_side::TransactionSide p_transaction_side);

	/**
	 * @returns Entry comment, i.e. descriptive text generally entered by
	 * the user.
	 */
	wxString comment() const;

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

	transaction_side::TransactionSide transaction_side() const;

	/**
	 * @returns the journal to which this Entry is attached.
	 *
	 * Note the PersistentJournalType must be known by the client, and must
	 * be specified as a template parameter. PersistentJournalType can ONLY
	 * be either OrdinaryJournal or DraftJournal. Otherwise, behaviour
	 * is undefined.
	 */
	template <typename PersistentJournalType>
	PersistentJournalType journal() const;

	/**
	 * @returns the posting date of the Entry, assuming it is associated
	 * with an OrdinaryJournal. If it is associated with another kind of
	 * Journal, then behaviour is undefined.
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


template <typename PersistentJournalType>
PersistentJournalType
Entry::journal() const
{
	return impl().journal<PersistentJournalType>();
}


}  // namespace phatbooks


#endif  // GUARD_entry_hpp_3389501093602507

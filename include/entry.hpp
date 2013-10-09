// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_hpp_7344880177334361
#define GUARD_entry_hpp_7344880177334361

#include "account.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_side.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <wx/string.h>
#include <memory>
#include <string>


namespace phatbooks
{

/**
 * Class representing an accounting entry, i.e. a single line in a journal,
 * i.e. a single change in the balance of an account.
 * 
 * Client code must deal with sqloxx::Handle<Entry> rather than with Entry
 * objects directly. This is enforced via "Signature" parameters
 * in the constructors for Account.
 */
class Entry:
	public sqloxx::PersistentObject<Entry, PhatbooksDatabaseConnection>
{
public:

	typedef sqloxx::PersistentObject<Entry, PhatbooksDatabaseConnection>
		PersistentObject;

	typedef sqloxx::IdentityMap<Entry> IdentityMap;
	
	/*
	 * Set up tables in the database required for the persistence of
	 * Entry objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	/**
	 * Construct a fresh Entry, not yet persisted to database.
	 *
	 * Client code cannot use this constructor - see class level
	 * documentation for why.
	 */
	Entry
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Get an Entry by Id from the database.
	 *
	 * Client cod cannot use this constructor - see class level
	 * documentation for why.
	 */
	Entry
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// Note copy constructor is private

	Entry(Entry&&) = delete;
	Entry& operator=(Entry const&) = delete;
	Entry& operator=(Entry&&) = delete;
	~Entry();

	/**
	 * Sets the journal_id for the Entry. Note this should \e not
	 * normally be called. The usual way to associate an Entry with a
	 * Journal (or DraftJournal or OrdinaryJournal) is for the Entry
	 * to be added to the Journal via its push_entry(...) method.
	 */
	void set_journal_id(sqloxx::Id p_journal_id);

	void set_account(sqloxx::Handle<Account> const& p_account);

	void set_comment(wxString const& p_comment);

	/**
	 * Set the amount of the Entry. The amount should be: a positive number
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
	 * TransactionSide).
	 */
	void set_transaction_side
	(	TransactionSide p_transaction_side
	);

	/**
	 * @returns comment, i.e. descriptive text generally entered by the user.
	 *
	 * Does not throw except possibly \c std::bad_alloc in
	 * extreme circumstances.
	 */
	wxString comment();

	/**
	 * @returns Entry amount (+ve for debits, -ve for credits).
	 *
	 * Note that, when the Account of the Entry is a P&L account,
	 * if we consider this P&L account from the point of view of a
	 * budgeting envelope, the \e amount() will be -ve if the Entry
	 * \e increases the
	 * available funds in the envelope,
	 * and +ve if the Entry \e decreases the available funds in an
	 * envelope.
	 *
	 * @todo Verify throwing behaviour.
	 */
	jewel::Decimal amount();

	sqloxx::Id journal_id();

	sqloxx::Handle<Account> account();

	bool is_reconciled();

	TransactionSide transaction_side();

	// Keep as std::string, for consistency with sqloxx
	static std::string exclusive_table_name();
	static std::string primary_key_name();

	/**
	 * Copy attributes of rhs to *this, but do \e not copy:\n
	 * \e id,\n
	 * \e database_connection, or \n
	 * \e journal_id.
	 */
	void mimic(Entry& rhs);

	/**
	 * @returns the posting date of the Entry, assuming it is associated
	 * with an OrdinaryJournal. If it is associated with another kind of
	 * Journal, then behaviour is undefined.
	 */
	boost::gregorian::date date();

private:

	void swap(Entry& rhs);

	/**
	 * Copy constructor - implemented, but deliberately private
	 */
	Entry(Entry const& rhs);

	void do_load() override;
	void do_save_existing() override;
	void do_save_new() override;
	void do_ghostify() override;
	void do_remove() override;
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct EntryData;

	std::unique_ptr<EntryData> m_data;

};


/**
 * @returns a unique_ptr to a heap-allocated SQLStatement from which
 * entry_id may be selected from the first result column. Contains
 * only Entries that belong to actual (i.e. non-budget)
 * Journals that are OrdinaryJournals (i.e. not DraftJournals).
 * Filtering may optionally be performed by Account and/or date.
 * Within the result set, Entries are ordered by date.
 */
std::unique_ptr<sqloxx::SQLStatement>
create_date_ordered_actual_ordinary_entry_selector
(	PhatbooksDatabaseConnection& p_database_connection,
	boost::optional<boost::gregorian::date> const& p_maybe_min_date =
		boost::optional<boost::gregorian::date>(),
	boost::optional<boost::gregorian::date> const& p_maybe_max_date =
		boost::optional<boost::gregorian::date>(),
	boost::optional<sqloxx::Handle<Account> > const& p_maybe_account =
		boost::optional<sqloxx::Handle<Account> >()
);

}  // namespace phatbooks

#endif  // GUARD_entry_hpp_7344880177334361

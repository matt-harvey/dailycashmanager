#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

/** \file entry.hpp
 *
 * \brief Header file pertaining to Entry class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "account.hpp"
#include "journal.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace sqloxx
{
	class SharedSQLStatement;
}  // namespace sqloxx


namespace phatbooks
{

class PhatbooksDatabaseConnection;

/**
 * Class representing an accounting entry, i.e. a single line in an account.
 */
class Entry:
	public sqloxx::PersistentObject<Entry, PhatbooksDatabaseConnection>
{
public:

	typedef sqloxx::PersistentObject<Entry, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;

	/**
	 * Sets up tables in the database required for the persistence of
	 * Entry objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	/**
	 * Initialize a "raw" entry, that will not yet correspond to any
	 * particular object in the database, and will not yet be associated
	 * with any particular Journal.
	 */
	explicit
	Entry
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection
	);

	/**
	 * Get an Entry by id from the database.
	 */
	Entry
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection,
		Id p_id
	);

	/**
	 * Destructor
	 */
	~Entry();

	/**
	 * Sets the journal_id for the Entry
	 */
	void set_journal_id(Journal::Id p_journal_id);

	/**
	 * Set the Account for the Entry.
	 */
	void set_account(Account const& p_account_id);

	/**
	 * Set the comment for the Entry
	 */
	void set_comment(std::string const& p_comment);

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
	 * @returns Entry comment.
	 * 
	 * Does not throw except possibly \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment();

	/**
	 * @returns Entry amount (+ve for debits, -ve for credits).
	 *
	 * @todo Verify throwing behaviour.
	 *
	 * Does not throw.
	 */
	jewel::Decimal amount();

	/**
	 * @returns id of the Account that this entry effects.
	 *
	 * Does not throw.
	 */
	Account account();

	/**
	 * @returns name of the Account that this entry effects.
	 *
	 * Note this is much less efficient than account_id().
	 *
	 * @todo Ascertain and document throwing behaviour.
	 */
	std::string account_name();
	
	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(Entry& rhs);

	static std::string primary_table_name();
private:

	/**
	 * Copy constructor - implemented, but deliberately private
	 */
	Entry(Entry const& rhs);
	
	void do_load();
	void do_save_existing();
	void do_save_new();
	void process_saving_statement(sqloxx::SharedSQLStatement& statement);

	struct EntryData
	{
		boost::optional<Journal::Id> journal_id;
		boost::optional<Account> account;
		boost::optional<std::string> comment;
		boost::optional<jewel::Decimal> amount;
	};

	boost::scoped_ptr<EntryData> m_data;

};



}  // namespace phatbooks

#endif  // GUARD_entry_hpp

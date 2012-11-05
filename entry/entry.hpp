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


#include "journal.hpp"
#include "general_typedefs.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

/**
 * Class representing an accounting entry, i.e. a single line in an account.
 */
class Entry: public sqloxx::PersistentObject<IdType>
{
public:

	typedef IdType Id;
	typedef sqloxx::PersistentObject<Id> PersistentObject;

	/**
	 * Sets up tables in the database required for the persistence of
	 * Entry objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);
	
	/**
	 * Initialize a "raw" entry, that will not yet correspond to any
	 * particular object in the database, and will not yet be associated
	 * with any particular Journal.
	 */
	explicit
	Entry
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
	);

	/**
	 * Get an Entry by id from the database.
	 */
	Entry
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		Id p_id
	);

	/**
	 * Sets the journal_id for the Entry
	 */
	void set_journal_id(Journal::Id p_journal_id);

	/**
	 * Set the Account name for the Entry.
	 *
	 * @todo Would it be better to store the Account id here?
	 *
	 * @todo Should this method throw if there is no Account with this name?
	 */
	void set_account_name(std::string const& p_account_name);

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
	 * @returns name of the Account that this entry effects.
	 *
	 * Does not throw, except possibly \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string account_name();
		
private:

	// Inherited virtual methods
	
	virtual void do_load_all();
	
	/**
	 * WARNING This needs to be implemented properly.
	 */
	virtual void do_save_existing_all()
	{
	}

	/**
	 * WARNING This needs to be implemented properly.
	 */
	virtual void do_save_existing_partial()
	{
	}

	virtual void do_save_new_all();

	virtual std::string do_get_table_name() const;

	// Data members
	boost::optional<Journal::Id> m_journal_id;
	boost::optional<std::string> m_account_name;
	boost::optional<std::string> m_comment;
	boost::optional<jewel::Decimal> m_amount;

};



}  // namespace phatbooks

#endif  // GUARD_entry_hpp

#ifndef GUARD_entry_impl_hpp
#define GUARD_entry_impl_hpp

/** \file entry.hpp
 *
 * \brief Header file pertaining to EntryImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "account.hpp"
#include "entry_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>



namespace phatbooks
{



/**
 * Class representing an accounting entry, i.e. a single line in an account.
 */
class EntryImpl:
	public sqloxx::PersistentObject<EntryImpl, PhatbooksDatabaseConnection>
{
public:

	typedef sqloxx::PersistentObject<EntryImpl, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;
	typedef sqloxx::IdentityMap<EntryImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	/**
	 * Sets up tables in the database required for the persistence of
	 * EntryImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	/**
	 * Initialize a "raw" entry, that will not yet correspond to any
	 * particular object in the database, and will not yet be associated
	 * with any particular Journal.
	 */
	explicit
	EntryImpl(IdentityMap& p_identity_map);

	/**
	 * Get an EntryImpl by id from the database.
	 */
	EntryImpl
	(	IdentityMap& p_identity_map,	
		Id p_id
	);

	/**
	 * Destructor
	 */
	~EntryImpl();

	/**
	 * Sets the journal_id for the EntryImpl
	 */
	void set_journal_id(sqloxx::Id p_journal_id);

	/**
	 * Set the Account for the EntryImpl.
	 */
	void set_account(Account const& p_account);

	/**
	 * Set the comment for the EntryImpl
	 */
	void set_comment(std::string const& p_comment);

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
	 * Set whether the EntryImpl has been reconciled (e.g. for
	 * reconciling to a bank or credit card statement).
	 */
	void set_whether_reconciled(bool p_is_reconciled);

	/**
	 * @returns EntryImpl comment.
	 * 
	 * Does not throw except possibly \c std::bad_alloc in
	 * extreme circumstances.
	 */
	std::string comment();

	/**
	 * @returns EntryImpl amount (+ve for debits, -ve for credits).
	 *
	 * @todo Verify throwing behaviour.
	 *
	 * Does not throw.
	 */
	jewel::Decimal amount();

	/**
	 * @returns the Account that this entry effects.
	 *
	 * Does not throw.
	 */
	Account account();

	/**
	 * @returns true if the Entry is reconciled, else false.
	 */
	bool is_reconciled();

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(EntryImpl& rhs);

	static std::string primary_table_name();
	static std::string primary_key_name();
private:

	/**
	 * Copy constructor - implemented, but deliberately private
	 */
	EntryImpl(EntryImpl const& rhs);
	
	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void do_remove();
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct EntryData;

	boost::scoped_ptr<EntryData> m_data;

};


struct EntryImpl::EntryData
{
	boost::optional<sqloxx::Id> journal_id;
	boost::optional<Account> account;
	boost::optional<std::string> comment;
	boost::optional<jewel::Decimal> amount;
	boost::optional<bool> is_reconciled;
};



}  // namespace phatbooks

#endif  // GUARD_entry_impl_hpp

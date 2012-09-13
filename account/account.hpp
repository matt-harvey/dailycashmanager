#ifndef GUARD_account_hpp
#define GUARD_account_hpp

/** \file account.hpp
 *
 * \brief Header file pertaining to Account class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "general_typedefs.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>


namespace phatbooks
{

/**
 * Represents an Account object that is "live" in memory, rather than
 * stored in a database.
 */
class Account: public PersistentObject<int>
{
public:

	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order - see
		// account_storage_manager.hpp
		revenue = 1,
		expense,
		asset,
		liability,
		pure_envelope
	};


	/**
	 * Returns a vector of account type names, corresponding to the
	 * AccountType enumerations, and in the same order.
	 */
	static std::vector<std::string> account_type_names();

	/**
	 * Sets up tables in the database required for the persistence of
	 * Account objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	// Default copy contructor is fine.
	// Default assignment is fine.
	// Default destructor is fine.

	/**
	 * Returns name of account.
	 */
	std::string name();

	/**
	 * Returns abbreviation of native commodity of this account.
	 */
	std::string commodity_abbreviation();

	/**
	 * Returns AccountType of account.
	 */
	AccountType account_type();

	/**
	 * Returns description of account.
	 */
	std::string description();

	void set_account_type();

	void set_name();

	void set_commodity_abbreviation();

	void set_description();

	
private:

	virtual void do_load_all();

	virtual void do_save_existing_all();

	virtual void do_save_existing_partial();

	virtual void do_save_new_all();

	virtual std::string do_get_table_name();

	boost::optional<std::string> m_name;

	// native commodity or currency of Account
	boost::optional<std::string> m_commodity_abbreviation;

	boost::optional<AccountType> m_account_type;

	boost::optional<std::string> m_description;

};


}  // namespace phatbooks


#endif  // GUARD_account_hpp

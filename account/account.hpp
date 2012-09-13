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
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace phatbooks
{

/**
 * Represents an Account object that is "live" in memory, rather than
 * stored in a database.
 *
 * @todo I should probably load m_name immediately with the id, but then
 * lazy load everything else. Note the m_name is used as an identifier
 * in Entry objects. At the moment I have got m_name as one of the lazy
 * attributes.
 */
class Account: public sqloxx::PersistentObject<int>
{
public:

	typedef IdType Id;
	typedef sqloxx::PersistentObject<Id> PersistentObject;

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

	/**
	 * Initialize a "draft" account, that will not correspond to any
	 * particular object in the database.
	 */
	explicit
	Account
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
	);
	
	/**
	 * Get an Account by id from database.
	 */
	Account
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		Id p_id
	);

	/**
	 * Get an Account by name from the database.
	 */
	Account
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		std::string const& p_name
	);

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

	void set_account_type(AccountType p_account_type);

	void set_name(std::string const& p_name);

	void set_commodity_abbreviation(std::string const& p_commodity_abbreviation);

	void set_description(std::string const& p_description);

	
private:

	virtual void do_load_all();

	/* WARNING I need to implement this properly
	 */
	virtual void do_save_existing_all()
	{
	}

	/* WARNING I need to implement this properly
	 */
	virtual void do_save_existing_partial()
	{
	}

	virtual void do_save_new_all();

	virtual std::string do_get_table_name();

	void load_name_knowing_id();

	void load_id_knowing_name();

	std::string m_name;

	// native commodity or currency of Account
	boost::optional<std::string> m_commodity_abbreviation;

	boost::optional<AccountType> m_account_type;

	boost::optional<std::string> m_description;

};


// Inline member functions


inline
Account::Account
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	PersistentObject(p_database_connection)
{
}

inline
Account::Account
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id)
{
	load_name_knowing_id();
}

inline
Account::Account
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	std::string const& p_name
):
	PersistentObject(p_database_connection),
	m_name(p_name)
{
	load_id_knowing_name();
	mark_as_persisted();
}


}  // namespace phatbooks


#endif  // GUARD_account_hpp

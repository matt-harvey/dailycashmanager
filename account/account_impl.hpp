#ifndef GUARD_account_impl_hpp
#define GUARD_account_impl_hpp

/** \file account_impl.hpp
 *
 * \brief Header file pertaining to AccountImpl class.
 *
 * \author Matthew Harvey
 * \date 06 Nov 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account.hpp"
#include "general_typedefs.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace phatbooks
{


/**
 * Represents an AccountImpl object that is "live" in memory, rather than
 * stored in a database.
 *
 * @todo I should probably load m_name immediately with the id, but then
 * lazy load everything else. Note the m_name is used as an identifier
 * in Entry objects. At the moment I have got m_name as one of the lazy
 * attributes.
 *
 * @todo This should inherit from a PersistentObjectImpl class, not
 * from PersistentObject - I think. Currently it's duplicating
 * the parent class component of Account class.
 */
class AccountImpl:
	public sqloxx::PersistentObject<IdType>,
	private boost::noncopyable
{
public:

	typedef IdType Id;
	typedef sqloxx::PersistentObject<Id> PersistentObject;
	typedef Account::AccountType AccountType;

	/**
	 * Returns a vector of account type names, corresponding to the
	 * AccountType enumerations, and in the same order.
	 */
	static std::vector<std::string> account_type_names();

	/**
	 * Sets up tables in the database required for the persistence of
	 * AccountImpl objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	/**
	 * Initialize a "draft" account, that will not correspond to any
	 * particular object in the database.
	 */
	explicit
	AccountImpl
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
	);
	
	/**
	 * Get an AccountImpl by id from database.
	 */
	AccountImpl
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		Id p_id
	);

	/**
	 * Get an AccountImpl by name from the database.
	 */
	AccountImpl
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

	void set_commodity_abbreviation
	(	std::string const& p_commodity_abbreviation
	);

	void set_description(std::string const& p_description);

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

	virtual std::string do_get_table_name() const;

	void load_name_knowing_id();

	void load_id_knowing_name();

private:

	std::string m_name;

	// native commodity or currency of Account
	boost::optional<std::string> m_commodity_abbreviation;

	boost::optional<AccountType> m_account_type;

	boost::optional<std::string> m_description;

};



// Inline member functions


inline
AccountImpl::AccountImpl
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	PersistentObject(p_database_connection)
{
}

inline
AccountImpl::AccountImpl
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id)
{
	load_name_knowing_id();
}

inline
AccountImpl::AccountImpl
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	std::string const& p_name
):
	PersistentObject(p_database_connection),
	m_name(p_name)
{
	load_id_knowing_name();
}


}  // namespace phatbooks

#endif  // GUARD_account_impl_hpp

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
#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <string>
#include <vector>


namespace sqloxx
{
	class SharedSQLStatement;
}  // namespace sqloxx


namespace phatbooks
{

/**
 * Represents an Account object that is "live" in memory, rather than
 * stored in a database.
 *
 * Derives from Loki::SmallValueObject<> for faster heap allocation.
 *
 * @todo I should probably load m_name immediately with the id, but then
 * lazy load everything else. Note the m_name is used as an identifier
 * in Entry objects. At the moment I have got m_name as one of the lazy
 * attributes.
 */
class Account:
	public sqloxx::PersistentObject
{
public:

	typedef sqloxx::PersistentObject PersistentObject;

	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order. See setup_tables
		// method
		asset = 1,
		liability,
		equity,
		revenue,
		expense,
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

	/**
	 * Destuctor.
	 */
	~Account();

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

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(Account& rhs);

private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	Account(Account const& rhs);

	void do_load();
	void do_save_existing();
	void do_save_new();
	std::string do_get_table_name() const;
	void load_name_knowing_id();
	void load_id_knowing_name();
	void process_saving_statement(sqloxx::SharedSQLStatement& statement);

	struct AccountData
	{
		// The optionals here are a safety feature. They
		// prevent uninitialized values from being
		// silently returned. Unitialized values are a feature
		// of the lazy loading pattern we are using here. They
		// are also a feature of objects newly created at the
		// presentation layer but not yet saved to the database
		// (and not intended to correspond to any object in the
		// database). Using optionals helps guard against
		// such an object being written to the database in an
		// incomplete state.
		std::string name;
		boost::optional<std::string> commodity_abbreviation;
		boost::optional<AccountType> account_type;
		boost::optional<std::string> description;
	};

	boost::scoped_ptr<AccountData> m_data;
};

}  // namespace phatbooks


#endif  // GUARD_account_hpp

#ifndef GUARD_account_impl_hpp
#define GUARD_account_impl_hpp

/** \file account.hpp
 *
 * \brief Header file pertaining to AccountImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account_type.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/identity_map.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <string>
#include <vector>


namespace sqloxx
{
	class SQLStatement;
}  // namespace sqloxx


namespace phatbooks
{


/**
 * Represents an AccountImpl object that is "live" in memory, rather than
 * stored in a database.
 */
class AccountImpl:
	public sqloxx::PersistentObject<AccountImpl, PhatbooksDatabaseConnection>
{
public:
	
	typedef sqloxx::Id Id;

	typedef sqloxx::PersistentObject<AccountImpl, PhatbooksDatabaseConnection>
		PersistentObject;

	typedef account_type::AccountType AccountType;
	typedef sqloxx::IdentityMap<AccountImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	/**
	 * Returns a vector of account type names, corresponding to the
	 * AccountType enumerations, and in the same order.
	 */
	static std::vector<std::string> account_type_names();

	/**
	 * Sets up tables in the database required for the persistence of
	 * AccountImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Returns the id of the account with name p_name.
	 */
	static Id id_for_name
	(	PhatbooksDatabaseConnection& dbc,
		std::string const& p_name
	);

	/**
	 * Initialize a "draft" account, that will not correspond to any
	 * particular object in the database.
	 */
	explicit
	AccountImpl(IdentityMap& p_identity_map);

	/**
	 * Get an AccountImpl by id from database.
	 */
	AccountImpl(IdentityMap& p_identity_map, Id p_id);

	/**
	 * Destuctor.
	 */
	~AccountImpl();

	/**
	 * Returns name of account.
	 */
	std::string name();

	/**
	 * Returns the native commodity of this account.
	 */
	Commodity commodity();

	/**
	 * Returns AccountImpl of account.
	 */
	AccountType account_type();

	/**
	 * Returns description of account.
	 */
	std::string description();

	/**
	 * Returns the balance of account.
	 */
	jewel::Decimal balance();

	void set_account_type(AccountType p_account_type);

	void set_name(std::string const& p_name);

	void set_commodity(Commodity const& p_commodity);

	void set_description(std::string const& p_description);

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(AccountImpl& rhs);

	static std::string primary_table_name();
	static std::string primary_key_name();
private:


	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	AccountImpl(AccountImpl const& rhs);

	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void do_remove();
	void process_saving_statement(sqloxx::SQLStatement& statement);

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
		boost::optional<std::string> name;
		boost::optional<Commodity> commodity;
		boost::optional<AccountType> account_type;
		boost::optional<std::string> description;
	};

	boost::scoped_ptr<AccountData> m_data;
};

}  // namespace phatbooks


#endif  // GUARD_account_impl_hpp

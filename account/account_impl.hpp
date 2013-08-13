// Copyright (c) 2013, Matthew Harvey. All rights reserved.

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
#include "b_string.hpp"
#include "budget_item.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/static_assert.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <boost/scoped_ptr.hpp>	
#include <algorithm>
#include <string>
#include <vector>





namespace phatbooks
{



namespace detail
{
	class AccountData;
}



/**
 * Implements Account.
 * Note multiple Account instances may share the same underlying AccountImpl.
 */
class AccountImpl:
	public sqloxx::PersistentObject<AccountImpl, PhatbooksDatabaseConnection>
{
public:
	
	typedef sqloxx::PersistentObject<AccountImpl, PhatbooksDatabaseConnection>
		PersistentObject;
	
	typedef typename PersistentObject::Id Id;

	// Other classes rely on the below static assertion being true.
	BOOST_STATIC_ASSERT((boost::is_same<Id, sqloxx::Id>::value));

	typedef account_type::AccountType AccountType;
	typedef account_super_type::AccountSuperType AccountSuperType;

	typedef sqloxx::IdentityMap<AccountImpl, PhatbooksDatabaseConnection>
		IdentityMap;


	/**
	 * Sets up tables in the database required for the persistence of
	 * AccountImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * @returns the Id of the AccountImpl with name p_name, matched
	 * case insensitively. If
	 * there are multiple such AccountImpls, then it is undefined
	 * which Id will be returned, but one will be.
	 *
	 * @throws InvalidAccountNameException if there is no AccountImpl
	 * named p_name (this is tested case insensitively).
	 */
	static Id id_for_name
	(	PhatbooksDatabaseConnection& dbc,
		BString const& p_name
	);

	/**
	 * Initialize a "draft" account, that will not correspond to any
	 * particular object in the database.
	 */
	explicit
	AccountImpl(IdentityMap& p_identity_map);

	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_name
	);

	static bool no_user_pl_accounts_saved
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	static bool none_saved_with_account_type
	(	PhatbooksDatabaseConnection& p_database_connection,
		account_type::AccountType p_account_type
	);

	static bool none_saved_with_account_super_type
	(	PhatbooksDatabaseConnection& p_database_connection,
		account_super_type::AccountSuperType p_account_super_type
	);

	/**
	 * Get an AccountImpl by id from database.
	 */
	AccountImpl(IdentityMap& p_identity_map, Id p_id);

	~AccountImpl();

	BString name();

	Commodity commodity();

	AccountType account_type();

	AccountSuperType account_super_type();

	BString description();

	jewel::Decimal technical_balance();

	jewel::Decimal friendly_balance();

	jewel::Decimal technical_opening_balance();

	jewel::Decimal friendly_opening_balance();
	
	jewel::Decimal budget();

	std::vector<BudgetItem> budget_items();

	void set_account_type(AccountType p_account_type);

	void set_name(BString const& p_name);

	void set_commodity(Commodity const& p_commodity);

	void set_description(BString const& p_description);

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(AccountImpl& rhs);

	// These are tied to SQLoxx API and must be std::string, not
	// BString.
	static std::string primary_table_name();
	static std::string exclusive_table_name();
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
		boost::optional<BString> name;
		boost::optional<Commodity> commodity;
		boost::optional<account_type::AccountType> account_type;
		boost::optional<BString> description;
	};

	boost::scoped_ptr<AccountData> m_data;
};





}  // namespace phatbooks


#endif  // GUARD_account_impl_hpp

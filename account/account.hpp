#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "account_impl.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <jewel/decimal.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class PhatbooksDatabaseConnection;


class Account
{
public:
	typedef account_type::AccountType AccountType;
	typedef sqloxx::Id Id;

	/**
	 * Returns a vector of account type names, corresponding to the
	 * AccountType enumerations, and in the same order.
	 */
	static std::vector<std::string> account_type_names();

	/**
	 * Sets up tables in the database required for the persistence of
	 * Account objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "draft" account, that will not correspond to any
	 * particular object in the database.
	 */
	explicit
	Account
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Get an Account by id from database. Throws if no
	 * such id.
	 */
	Account
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * Get an Account by id from the database (fast,
	 * unchecked form).
	 */
	static Account create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * Get an Account by name from the database.
	 */
	Account
	(	PhatbooksDatabaseConnection& p_database_connection,
		std::string const& p_name
	);

	/**
	 * Destuctor.
	 */
	~Account();

	/**
	 * TODO This should eventually be shifted into a base
	 * class.
	 */
	Id id() const;

	/**
	 * TODO This should eventually be shifted into a base
	 * class.
	 */
	void save();

	/**
	 * Returns name of account.
	 */
	std::string name() const;

	/**
	 * Returns native commodity of this account.
	 */
	Commodity commodity() const;

	/**
	 * Returns AccountType of account.
	 */
	AccountType account_type() const;

	/**
	 * Returns description of account.
	 */
	std::string description() const;

	jewel::Decimal technical_balance() const;

	jewel::Decimal friendly_balance() const;

	void set_account_type(AccountType p_account_type);

	void set_name(std::string const& p_name);

	void set_commodity(Commodity const& p_commodity);

	void set_description(std::string const& p_description);

private:
	Account(sqloxx::Handle<AccountImpl> const& p_handle);
	sqloxx::Handle<AccountImpl> m_impl;

};




}  // namespace phatbooks

#endif  // GUARD_account_hpp

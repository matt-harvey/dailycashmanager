#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "account_type.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <string>
#include <vector>

namespace phatbooks
{

class AccountImpl;
class PhatbooksDatabaseConnection;


class Account
{
public:
	typedef sqloxx::Id Id;
	typedef account_type::AccountType AccountType;

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
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection
	);

	/**
	 * Get an Account by id from database.
	 */
	Account
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection,
		Id p_id
	);

	/**
	 * Get an Account by name from the database.
	 */
	Account
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection,
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
	 * Returns id of native commodity of this account.
	 */
	Id commodity_id() const;

	/**
	 * Returns abbreviation of native commodity of this account.
	 *
	 * Note this is much less efficient than commodity_id().
	 */
	std::string commodity_abbreviation() const;

	/**
	 * Returns AccountType of account.
	 */
	AccountType account_type() const;

	/**
	 * Returns description of account.
	 */
	std::string description() const;

	void set_account_type(AccountType p_account_type);

	void set_name(std::string const& p_name);

	void set_commodity_id(Id p_commodity_id);

	void set_description(std::string const& p_description);

private:

	sqloxx::Handle<AccountImpl> m_impl;

};



}  // namespace phatbooks

#endif  // GUARD_account_hpp

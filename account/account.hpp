#ifndef GUARD_account_hpp
#define GUARD_account_hpp

#include "account_impl.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "budget_item.hpp"
#include "phatbooks_persistent_object.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class Commodity;
class PhatbooksDatabaseConnection;


/**
 * Represents an account - which could be either a balance sheet
 * or a P&L account.
 *
 * @todo Provide a mechanism for user to hide an Account if they so
 * choose (or to delete it, providing it has no Entries in it). The
 * user should still be able to access and unhide the Account later.
 * (In the TUI they should be able to see all Accounts in the detailed
 * Account list, with a Column indicating for each whether it is
 * visible or hidden). The user should probably not be able to hide Accounts
 * that have a non-zero balance, though.
 */
class Account:
	public PhatbooksPersistentObject<AccountImpl>
{
public:
	typedef account_type::AccountType AccountType;
	typedef account_super_type::AccountSuperType AccountSuperType;
	typedef
		PhatbooksPersistentObject<AccountImpl>
		PhatbooksPersistentObject;
	typedef PhatbooksPersistentObjectBase::Id Id;


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
		BString const& p_name
	);

	/**
	 * @returns \e true if and only if \e p_name is the name of an Account
	 * stored in the database.
	 *
	 * @param p_name name of Account.
	 *
	 * @throws SQLiteException or an exception derived therefrom, if
	 * something goes wrong with the SQL involved in executing
	 * the function. This might occur if \c p_name contains punctuation
	 * marks, spaces or etc., or is a SQL keyword, so that it cannot validly
	 * be a string parameter in a SQL statement. A SQLiteException or
	 * derivative might also be thrown if the database connection is invalid
	 * or the database corrupt. It is not guaranteed that these are the only
	 * circumstances in which an exception might be thrown.
	 */
	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_name
	);

	/**
	 * @returns \e true if the only P&L Account that is saved is
	 * p_database_connection.balancing_account(), or if there are no
	 * P&L Accounts saved at all. If there is one or more P&L Account saved
	 * other than p_database_connection.balancing_account(), then
	 * this function returns \e false.
	 */
	static bool no_user_pl_accounts_saved
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * @returns \e true if and only if there are \e no instances of Account
	 * with account type \e p_account_type saved in the database connected to
	 * by p_database_connection.
	 */
	static bool none_saved_with_account_type
	(	PhatbooksDatabaseConnection& p_database_connection,
		account_type::AccountType p_account_type
	);

	/**
	 * @returns \e true if and only if there \e no instances of Account
	 * with account super type \e p_account_super_type saved in the database
	 * connected to be p_database_connection.
	 */
	static bool none_saved_with_account_super_type
	(	PhatbooksDatabaseConnection& p_database_connection,
		account_super_type::AccountSuperType p_account_super_type
	);

	static void setup_tables();

	~Account();

	/**
	 * @ returns name of account
	 */
	BString name() const;

	/**
	 * @ returns native Commodity of this account
	 */
	Commodity commodity() const;

	/**
	 * @ returns AccountType of account.
	 */
	AccountType account_type() const;

	/**
	 * @returns AccountSuperType of account.
	 */
	AccountSuperType account_super_type() const;

	/**
	 * @ returns description of account.
	 */
	BString description() const;

	/**
	 * @returns "technical" account balance, which is
	 * positive for debit balances and negative for
	 * credit balances. For P&L accounts this
	 * corresponds to the unspent funds in the envelope,
	 * where a negative balance indicates that funds
	 * remain unspent, and a positive balance indicates
	 * that more funds have been spend than were
	 * available.
	 */
	jewel::Decimal technical_balance() const;

	/**
	 * @returns "user friendly" account balance, which, for
	 * P&L accounts, has the signs reversed relative to
	 * technical_balance().
	 */
	jewel::Decimal friendly_balance() const;

	/**
	 * @returns "technical" opening balance for the
	 * Accouint, which is positive for debit balances
	 * and negative for credit balances. Note that
	 * for balance sheet Accounts, this represents
	 * the actual opening balance of the Account; but
	 * for P&L Accounts, this represents the initial
	 * allocation of budget funds into the "envelope"
	 * for this Account (which will generally be a credit),
	 */
	jewel::Decimal technical_opening_balance() const;

	/**
	 * friendly_opening_balance() is to technical_opening_balance()
	 * what friendly_balance() it to technical_balance().
	 */
	jewel::Decimal friendly_opening_balance() const;

	/**
	 * @returns the amount of the recurring budget for
	 * the account, in terms of the standard
	 * Frequency for database_connection().
	 * E.g. if the standard Frequency is daily, then
	 * then a budget() of 100.00 implies 100.00/day.
	 */
	jewel::Decimal budget() const;

	/**
	 * @returns a std::vector containing all and only
	 * the saved BudgetItems for this Account.
	 */
	std::vector<BudgetItem> budget_items() const;


	void set_account_type(AccountType p_account_type);

	void set_name(BString const& p_name);

	void set_commodity(Commodity const& p_commodity);

	void set_description(BString const& p_description);
	
	/**
	 * Set the opening balance of the Account. This
	 * should be set in terms of the technical opening
	 * balance, not the friendly opening balance (see
	 * corresponding getters for more information about
	 * the difference).
	 */
	void set_opening_balance(jewel::Decimal const& p_opening_balance);


private:
	Account(sqloxx::Handle<AccountImpl> const& p_handle);
};


// This has to be std::string as it interfaces with Consolixx
// which requires it to be std::string
boost::shared_ptr<std::vector<std::string> >
make_account_row(Account const& account);

// This has to be std::string as it interfaces with Consolixx
// which requires it to be std::string
boost::shared_ptr<std::vector<std::string> >
make_detailed_account_row(Account const& account);


// Free-standing functions
bool is_asset_or_liability(Account const& account);
bool is_balance_sheet_account(Account const& account);
bool is_expense(Account const& account);
bool is_revenue(Account const& account);
bool is_pl_account(Account const& account);
bool is_not_pure_envelope(Account const& account);
std::vector<account_type::AccountType> balance_sheet_account_types();
std::vector<account_type::AccountType> pl_account_types();



}  // namespace phatbooks

#endif  // GUARD_account_hpp

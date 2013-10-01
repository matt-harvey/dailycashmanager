// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_hpp_5971945187063862
#define GUARD_account_hpp_5971945187063862

/** \file account_handle.hpp
 *
 * \brief Header file pertaining to Account class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account_type_fwd.hpp"
#include "budget_item_handle_fwd.hpp"
#include "commodity_handle_fwd.hpp"
#include "date.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_conv.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <boost/optional.hpp>
#include <wx/string.h>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>


namespace phatbooks
{


/**
 * Represents an account - which could be either a balance sheet account,
 * or an P&L account. The latter is equivalent to an "envelope" or
 * "category".
 * 
 * Client code must deal with AccountHandles rather than with Account
 * objects directly. This is enforced via "Signature" parameters
 * in the constructors for Account.
 */
class Account:
	public sqloxx::PersistentObject<Account, PhatbooksDatabaseConnection>
{
public:
	
	typedef sqloxx::PersistentObject<Account, PhatbooksDatabaseConnection>
		PersistentObject;

	typedef sqloxx::IdentityMap<Account, PhatbooksDatabaseConnection>
		IdentityMap;

	/**
	 * Sets up tables in the database required for the persistence of
	 * Account objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * @returns the Id of the Account with name p_name, matched
	 * case insensitively. If
	 * there are multiple such Account, then it is undefined
	 * which Id will be returned, but one will be.
	 *
	 * @throws InvalidAccountNameException if there is no Account
	 * named p_name (this is tested case insensitively).
	 */
	static sqloxx::Id id_for_name
	(	PhatbooksDatabaseConnection& dbc,
		wxString const& p_name
	);

	/**
	 * Initialize a "draft" Account, that will not correspond to any
	 * particular object in the database.
	 *
	 * The Signature parameter ensures that this can only be called by
	 * IdentityMap. Ordinary client code should use AccountHandle,
	 * not Account.
	 */
	Account
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Get an Account by id from database.
	 *
	 * The Signature parameter esures that this can only be called
	 * by IdentityMap. Ordinary client code should use AccountHandle,
	 * not Account.
	 */
	Account
	(	IdentityMap& p_identity_map,
		sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	static bool exists
	(	PhatbooksDatabaseConnection& p_database_connection,
		sqloxx::Id p_id
	);

	/**
	 * @returns \e true if and only if \e p_name is the name of an Account
	 * stored in the database, matched case insensitively.
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
		wxString const& p_name
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
		AccountType p_account_type
	);

	/**
	 * @returns \e true if and only if there are \e no instances of Account
	 * with account super type \e p_account_super_type saved in the database
	 * connection to by p_database_connection.
	 */
	static bool none_saved_with_account_super_type
	(	PhatbooksDatabaseConnection& p_database_connection,
		AccountSuperType p_account_super_type
	);


	// copy constructor is private

	Account(Account&&) = delete;
	Account& operator=(Account const&) = delete;
	Account& operator=(Account&&) = delete;
	~Account();

	wxString name();

	CommodityHandle commodity();

	AccountType account_type();

	AccountSuperType account_super_type();

	wxString description();

	Visibility visibility();

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
	jewel::Decimal technical_balance();

	/**
	 * @returns "user friendly" account balance, which, for
	 * P&L accounts only, has the signs reversed relative to
	 * technical_balance().
	 */
	jewel::Decimal friendly_balance();

	/**
	 * @returns "technical" opening balance of Account.
	 * See documentation for technical_balance() and
	 * friendly_balance(), for explanation
	 * of difference between "technical" and "friendly"
	 * balance.
	 */
	jewel::Decimal technical_opening_balance();

	/**
	 * @returns "user-friendly" opening balance of Account,
	 * See documentation for technical_balance() and
	 * friendly_balance(), for explanation
	 * of difference between "technical" and "friendly"
	 * balance.
	 */
	jewel::Decimal friendly_opening_balance();

	/**
	 * @returns the amount of the recurring budget for
	 * the account, in terms of the standard
	 * Frequency for database_connection().
	 * E.g. if the standard Frequency is daily, then
	 * then a budget() of 100.00 implies 100.00/day.
	 */
	jewel::Decimal budget();

	/**
	 * @returns a std::vector containing all and only
	 * the saved BudgetItems for this Account.
	 */
	std::vector<BudgetItemHandle> budget_items();

	void set_account_type(AccountType p_account_type);

	void set_name(wxString const& p_name);

	void set_commodity(CommodityHandle const& p_commodity);

	void set_description(wxString const& p_description);

	void set_visibility(Visibility p_visibility);

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(Account& rhs);

	// These are tied to SQLoxx API and must be std::string, not
	// wxString.
	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	Account(Account const& rhs);

	void do_load() override;
	void do_save_existing() override;
	void do_save_new() override;
	void do_ghostify() override;
	void do_remove() override;
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct AccountData;

	std::unique_ptr<AccountData> m_data;
};


// NON-MEMBER FUNCTIONS

/**
 * Type used to pass options to \e account_concept_name and
 * \e account_concepts_phrase functions. For interface, see
 * documentation for \e jewel::FlagSet, in the Jewel library.
 */
typedef
	jewel::FlagSet
	<	string_flags::StringFlags,
		string_flags::capitalize |
		string_flags::include_article |
		string_flags::pluralize
	>
	AccountPhraseFlags;

/**
 * @returns "account", "category" or some such string to describe
 * to the \e user the "thing" which they are creating in this
 * particular MultiAccountPanel.
 */
wxString account_concept_name
(	AccountSuperType p_account_super_type,
	AccountPhraseFlags p_phrase_flag_set = AccountPhraseFlags()
);

/**
 * @returns a string of the form "X or Y", where "X" and "Y" are the names
 * of the different possible "account concepts" (e.g. this might
 * return something like "account or category").
 */
wxString account_concepts_phrase
(	AccountPhraseFlags p_phrase_flag_set = AccountPhraseFlags()
);

/**
 * @returns a map which indicates, for each AccountSuperType,
 * the Account of that AccountSuperType which has the largest number of
 * ActualOrdinaryEntries (however, the budget balancing Account is
 * never included, nor are any pure_envelope Accounts).
 */
std::map<AccountSuperType, sqloxx::Id>
favourite_accounts(PhatbooksDatabaseConnection& p_database_connection);


}  // namespace phatbooks


#endif  // GUARD_account_hpp_5971945187063862

#ifndef GUARD_phatbooks_database_connection_hpp
#define GUARD_phatbooks_database_connection_hpp

/** \file phatbooks_database_connection.hpp
 *
 * \brief Header file pertaining to PhatbooksDatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account.hpp"
#include "database_connection.hpp"
#include "commodity_storage_manager.hpp"
#include <boost/bimap.hpp>
#include <string>

namespace phatbooks
{

// Forward declarations
class Account;
class Commodity;
class Journal;


/** Phatbooks-specific database connection class.
 * See API documentation for sqloxx::DatabaseConnection,
 * for parts of API inherited from sqloxx::DatabaseConnection.
 *
 * @todo Factor out code common to different \c store functions.
 *
 * @todo If speed becomes a problem, I should find a way to avoid
 * having to call sqlite3_prepare_v2, by caching previously prepared
 * sqlite3_stmt* somehow. I think the only feasible way would be to store it
 * as a static member
 * variable of DatabaseConnection. That way the sqlite3_stmt can
 * be freed using sqlite3_finalize in the destructor.
 * There would end up being several of the pre-prepared statements.
 * I tried making the prepared SQL statement a static variable in the
 * \c function, but this meant I couldn't call sqlite3_finalize on the
 * statement at the appropriate time.
 * Having said this, there's no point unless
 * it causes a problem by being too slow.
 */
class PhatbooksDatabaseConnection:
	public sqloxx::DatabaseConnection
{
public:
	/**
	 * Exhibits the same throwing behaviour (if any) as
	 * default constructor for sqloxx::DatabaseConnection.
	 */
	PhatbooksDatabaseConnection();
	
	/** Store an Account object in the database
	 * 
	 * @todo Verify that throwing behaviour is as documented.
	 * 
	 * @throws StoragePreconditionsException if p_account has invalid
	 * commodity abbreviation. (Commodity abbreviation of
	 * p_account must correspond to a commodity that has already
	 * been stored in the database.)
	 *
	 * @throws sqloxx::NoPrimaryKeyException if the table for storing
	 * Account data does not have a primary key.
	 *
	 * @throws sqloxx::CompoundPrimaryKeyException if the table for storing
	 * Account data has a compound primary key.
	 * 
	 * @throws sqloxx::TableSizeException if the table has reached its
	 * maximum size and therefore cannot accept any additional rows.
	 *
	 * @throws sqloxx:DatabaseExceptions or some derivative thereof, if
	 * something else goes wrong in finding the primary key to be
	 * assigned to the inserted object. (This is not expected ever to occur
	 * unless there is a heap allocation issue or the database is corrupt.)
	 *
	 * @throws sqloxx::InvalidConnection if the database connection is invalid
	 * at the time the storage is attempted.
	 *
	 * @throws sqloxx::SQLiteException or some derivative thereof, if there
	 * is some error binding the data for \c p_account to the SQL statement
	 * involved in storing the account, or in executing the
	 * resulting SQL statement. This not expected to occur except
	 * in cases of a corrupt database or memory allocation failure.
	 *
	 * @throws sqloxx::ConstraintException if there are multiple commodities
	 * with the same commodity abbreviation as the commodity abbreviation
	 * of \c p_account (It is not expected this will occur except case of
	 * a corrupt database, or a bug in Phatbooks.)
	 *
	 * @param p_account the Account to be stored.
	 *
	 * @returns the integer primary key of the Account just stored.
	 * (This is an autoincrementing primary key.)
	 */
	IdType store(Account const& p_account);

	/**
	 * @todo Figure out and document throwing behaviour.
	 *
	 * @todo Resolve issue that this does not take draft journals
	 * into account.
	 */
	IdType store(Journal const& p_journal);

	/**
	 * @returns \c true if and only if \c p_name is the name of an Account
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
	bool has_account_named(std::string const& p_name);

	/**
	 * @returns a boost::bimap from AccountType to string, giving the
	 * string name for each AccountType.
	 *
	 * @todo Determine and document throwing behaviour.
	 */
	boost::bimap<Account::AccountType, std::string> account_types();

	/**
	 * @returns \c true if and only if \c p_abbreviation is the abbreviation
	 * of a Commodity stored in the database.
	 */
	bool has_commodity_with_abbreviation(std::string const& p_abbreviation);

	/**
	 * @returns \c true if and only if \c p_name is the name of a Commodity
	 * stored in the database.
	 */
	bool has_commodity_named(std::string const& p_name);

	/**
	 * @returns the Commodity of an Account given the name of that Account.
	 *
	 * @todo HIGH PRIORITY Make it exception safe and determine and document
	 * throwing behaviour.
	 */
	Commodity commodity_for_account_named(std::string const& account_name);

	/**
	 * Creates tables required for Phatbooks, and inserts rows
	 * into certain tables to provide application-level data where
	 * required. If the database already contains these tables,
	 * the function does nothing. This function should always be
	 * called after calling DatabaseConnection::open.
	 *
	 * @throws SQLiteException or some derivative thereof, if setup is
	 * unsuccessful.
	 */
	void setup();

private:
	bool setup_has_occurred();
	static std::string const s_setup_flag;

};  // PhatbooksDatabaseConnection



}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp

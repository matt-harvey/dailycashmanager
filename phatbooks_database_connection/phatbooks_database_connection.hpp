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


#include "account_type.hpp"
#include "b_string.hpp"
#include "frequency.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>



namespace sqloxx
{
	// Forward declaration
	template <typename T, typename Connection>
	class IdentityMap;
}


namespace phatbooks
{

// Forward declarations
class BalanceCache;
class AmalgamatedBudget;
class AccountImpl;
class BudgetItemImpl;
class CommodityImpl;
class EntryImpl;
class DraftJournalImpl;
class OrdinaryJournalImpl;
class RepeaterImpl;


/**
 * Phatbooks-specific database connection class.
 * See API documentation for sqloxx::DatabaseConnection,
 * for parts of API inherited from sqloxx::DatabaseConnection.
 *
 * A PhatbooksDatabaseConnection represents a connection to a
 * particular database in which data is stored for a particular
 * accounting entity. Since there is only on accounting entity
 * per database - and there is no distinct "Entity" class in
 * the Phatbooks object model - we can generally treat
 * a PhatbooksDatabaseConnection as a proxy for the accounting
 * entity itself.
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
	
	~PhatbooksDatabaseConnection();

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

	/**
	 * Set degree of caching of objects loaded from database.
	 *
	 * Level 0 entails nil caching (except temporary caching required to
	 * avoid
	 * loading of duplicate objects).
	 *
	 * Level 5 entails some caching of objects of which there are only a few
	 * instances expected to exist in the database.
	 *
	 * Level 10 entails maximum caching.
	 *
	 * If \e level is not one of the specific levels listed above, the effect
	 * will be the same as the next lowest level that is listed above.
	 *
	 * When the caching level is changed from one significant level to another
	 * significant level that is lower than the first, any objects of classes
	 * that are not cached under the new level, that were cached under the old
	 * level, that are cached at the time the level changes, are emptied from
	 * the cache.
	 *
	 * @todo Determinate and document throwing behaviour.
	 */
	void set_caching_level(unsigned int level);

	/**
	 * Class to provide restricted access to cache holding
	 * Account balances.
	 */
	class BalanceCacheAttorney
	{
	public:
		friend class AccountImpl;
		friend class CommodityImpl;
		friend class EntryImpl;
	private:
		// Mark whole balance cache as stale
		static void mark_as_stale
		(	PhatbooksDatabaseConnection const& p_database_connection
		);
		// Mark a particular Account in the balance cache as stale
		// NOTE: Ideally we should have AccountImpl::Id here, or perhaps
		// Account::Id (rather than sqloxx::Id). However this cannot be
		// achieved without #including either account.hpp or account_impl.hpp.
		// Doing this results in circular #includes. It is simpler just to use
		// sqloxx::Id here. A static assertion has been placed in
		// account_impl.hpp, to ensure that AccountImpl::Id is always the same
		// type as sqloxx::Id anyway.
		static void mark_as_stale
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
		// Retrieve the technical_balance of an Account
		static jewel::Decimal technical_balance
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
	};
	friend class BalanceCacheAttorney;

	Frequency budget_frequency() const;

	jewel::Decimal budget_balance() const;

	/**
	 * Class to provide restricted access to cache in which AmalgamatedBudget
	 * is stored.
	 */
	class BudgetAttorney
	{
	public:
		friend AccountImpl;
		friend BudgetItemImpl;
		friend PhatbooksDatabaseConnection;
	private:
		// Regenerate the AmalgamatedBudget, and its associated
		// "instrument" DraftJournal, on the basis of the currently
		// saved BudgetItems.
		static void regenerate
		(	PhatbooksDatabaseConnection const& p_database_connection
		);
		// Retrieve the amalgamated budget for a given Account,
		// expressed in terms of the standard Frequency of the
		// AmalgamatedBudget for this PhatbooksDatabaseConnection.
		// NOTE: Ideally we should have AccountImpl::Id here, or perhaps
		// Account::Id (rather than sqloxx::Id). However this cannot be
		// achieved without #including either account.hpp or account_impl.hpp.
		// Doing this results in circular #includes. It is simpler just to use
		// sqloxx::Id here. A static assertion has been placed in
		// account_impl.hpp, to ensure that AccountImpl::Id is always the same
		// type as sqloxx::Id anyway.
		static jewel::Decimal budget
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
	};
	friend class BudgetAttorney;

	template<typename T>
	sqloxx::IdentityMap<T, PhatbooksDatabaseConnection>& identity_map();

private:

	bool setup_has_occurred();
	void mark_setup_as_having_occurred();

	BalanceCache* m_balance_cache;
	AmalgamatedBudget* m_budget;

	sqloxx::IdentityMap<AccountImpl, PhatbooksDatabaseConnection>*
		m_account_map;
	sqloxx::IdentityMap<BudgetItemImpl, PhatbooksDatabaseConnection>*
		m_budget_item_map;
	sqloxx::IdentityMap<CommodityImpl, PhatbooksDatabaseConnection>*
		m_commodity_map;
	sqloxx::IdentityMap<EntryImpl, PhatbooksDatabaseConnection>*
		m_entry_map;
	sqloxx::IdentityMap<OrdinaryJournalImpl, PhatbooksDatabaseConnection>*
		m_ordinary_journal_map;
	sqloxx::IdentityMap<DraftJournalImpl, PhatbooksDatabaseConnection>*
		m_draft_journal_map;
	sqloxx::IdentityMap<RepeaterImpl, PhatbooksDatabaseConnection>*
		m_repeater_map;



};  // PhatbooksDatabaseConnection


}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp

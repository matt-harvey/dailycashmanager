/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_phatbooks_database_connection_hpp_19608494974490487
#define GUARD_phatbooks_database_connection_hpp_19608494974490487

#include "account_type.hpp"
#include "frequency.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <sqloxx/database_connection.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/identity_map_fwd.hpp>
#include <jewel/decimal.hpp>
#include <list>
#include <string>


namespace phatbooks
{

// Begin forward declarations

class AmalgamatedBudget;
class AmalgamatedBudgetSignature;
class Account;
class BalanceCache;
class BudgetItem;
class Commodity;
class DraftJournal;
class Entry;
class PersistentJournal;
class Repeater;

// End forward declarations


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

	PhatbooksDatabaseConnection(PhatbooksDatabaseConnection const&) = delete;
	PhatbooksDatabaseConnection(PhatbooksDatabaseConnection&&) = delete;
	PhatbooksDatabaseConnection& operator=
	(	PhatbooksDatabaseConnection const&
	) = delete;
	PhatbooksDatabaseConnection& operator=
	(	PhatbooksDatabaseConnection&&
	) = delete;

	~PhatbooksDatabaseConnection();

	/**
	 * @returns the date on which the database was created. This notionally
	 * corresponds to the date on which the accounting entity was
	 * created.
	 */
	boost::gregorian::date entity_creation_date() const;

	/**
	 * @returns the date which all, and only, the <em>opening balance</em>
	 * journals are dated at, for this entity.
	 */
	boost::gregorian::date opening_balance_journal_date() const;

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
	 */
	void set_caching_level(unsigned int level);

	/**
	 * @returns the sqloxx::Handle<Account> to which budget imbalances are
	 * reconciled in the budget_instrument().
	 */
	sqloxx::Handle<Account> balancing_account() const;

	/**
	 * @returns a handle to the default Commodity for the entity represented by
	 * the PhatbooksDatabaseConnection.
	 */
	sqloxx::Handle<Commodity> default_commodity() const;

	/**
	 * Set the default commodity for the entity to p_commodity. If this
	 * is set before the database connection has been opened to a
	 * database, then, when the connection \e is opened, the Commodity
	 * that was set as the default Commodity will be automatically
	 * saved to the database at that time.
	 *
	 * @throws InvalidDefaultCommodityException when p_commodity has
	 * a multiplier_to_base that is not equal to Decimal(1, 0).
	 *
	 * @throws jewel::UninitializedOptionalException if p_commodity
	 * does not have an initialized multiplier_to_base.
	 *
	 * If this throws an exception, then the database state will be as it was
	 * prior to the function being called, and the in-memory state of the
	 * PhatbooksDatabaseConnection and of the
	 * default Commodity will be virtually as it was (as far as client code
	 * is concerned).
	 *
	 * If this throws sqloxx::UnresolvedTransactionException (extremely
	 * unlikely), then the program
	 * should be gracefully exited prior to further SQL being executed on
	 * the database connection.
	 */
	void set_default_commodity(sqloxx::Handle<Commodity> const& p_commodity);

	/**
	 * @returns the DraftJournal that serves as the "instrument"
	 * by means of which the AmalgamatedBudget (for the accounting
	 * entity represented by the PhatbooksDatabaseConnection) effects regular
	 * distributions of budget amounts to budgeting envelopes
	 * (Accounts).
	 *
	 * NOTE Client code should access this only with care. The
	 * "instrument" should normally be modified / managed only
	 * by AmalgamatedBudget.
	 */
	sqloxx::Handle<DraftJournal> budget_instrument() const;
	
	/**
	 * Class to provide restricted access to cache holding Account balances.
	 */
	class BalanceCacheAttorney
	{
	public:
		friend class Account;
		friend class Commodity;
		friend class Entry;
		BalanceCacheAttorney() = delete;
		~BalanceCacheAttorney() = delete;
	private:
		// Mark whole balance cache as stale.
		static void mark_as_stale
		(	PhatbooksDatabaseConnection const& p_database_connection
		);
		static void mark_as_stale
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
		// Retrieve the technical_balance of an Account
		static jewel::Decimal technical_balance
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
		// Retrieve the technical opening balance of an Account
		static jewel::Decimal technical_opening_balance
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
	};
	friend class BalanceCacheAttorney;

	Frequency budget_frequency() const;

	jewel::Decimal budget_balance() const;

	bool supports_budget_frequency(Frequency const& p_frequency) const;

	/**
	 * Class to provide restricted access to cache in which AmalgamatedBudget
	 * is stored.
	 */
	class BudgetAttorney
	{
	public:
		friend class Account;
		friend class BudgetItem;
		friend class PhatbooksDatabaseConnection;
		BudgetAttorney() = delete;
		~BudgetAttorney() = delete;
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
		static jewel::Decimal budget
		(	PhatbooksDatabaseConnection const& p_database_connection,
			sqloxx::Id p_account_id
		);
	};
	friend class BudgetAttorney;

	template<typename T>
	sqloxx::IdentityMap<T>& identity_map();


private:

	/**
	 * Overrides sqloxx::DatabaseConnection::do_setup(). Will be called
	 * as final step in execution of open().
	 *
	 * Creates tables required for Phatbooks, and inserts rows
	 * into certain tables to provide application-level data where
	 * required - if this has not already occurred (this step is
	 * ignored if the database has already been configured for
	 * Phatbooks).
	 *
	 * Any "entity level" data is then loaded into memory where required.
	 *
	 * @throws SQLiteException or some derivative thereof, if setup is
	 * unsuccessful.
	 */
	void do_setup() override;

	void setup_entity_table();
	bool tables_are_configured();
	void mark_tables_as_configured();

	/**
	 * Cache certain data relating to the accounting entity, where the
	 * data is generally unchanging and stored permanently in the database -
	 * we just load it here for (a) quick access, but also (b) so that
	 * the getters for these data, in the PhatbooksDatabaseConnection itself -
	 * can be const: we have to load them separately as we can't create a
	 * SQLStatement
	 * on a const DatabaseConnection. Se we load it separately
	 * here as part of do_setup().
	 *
	 * Note an instance of PermanentEntityData has no connection to
	 * a database; it is \e just a cache; the PhatbooksDatabaseConnection
	 * must separately manage the persistence of this data to the
	 * database.
	 */
	class PermanentEntityData;

	/**
	 * Load creation date from the database into memory.
	 */
	void load_creation_date();

	/**
	 * Load default commodity from the database into memory.
	 */
	void load_default_commodity();

	/**
	 * Persist to the database the default Commodity. Note that as well
	 * as calling save() on the default Commodity, this also stores in
	 * the database the fact that \e this Commodity is the \e default
	 * Commodity.
	 *
	 * If this throws an exception, then the database state will be as it was
	 * prior to the function being called, and the in-memory state of the
	 * PhatbooksDatabaseConnection and of the
	 * default Commodity will be virtually as it was (as far as client code
	 * is concerned).
	 *
	 * If this throws sqloxx::UnresolvedTransactionException (extremely
	 * unlikely), then the program
	 * should be gracefully exited prior to further SQL being executed on
	 * the database connection.
	 */
	void save_default_commodity();

	// Using raw pointers here rather than smart pointers, as we want
	// to be able easily to verify within the body of the destructor, the
	// order of deletion of pointer members.
	PermanentEntityData* m_permanent_entity_data;
	BalanceCache* m_balance_cache;
	AmalgamatedBudget* m_budget;
	sqloxx::IdentityMap<Account>* m_account_map;
	sqloxx::IdentityMap<BudgetItem>* m_budget_item_map;
	sqloxx::IdentityMap<Commodity>* m_commodity_map;
	sqloxx::IdentityMap<Entry>* m_entry_map;
	sqloxx::IdentityMap<PersistentJournal>* m_journal_map;
	sqloxx::IdentityMap<Repeater>* m_repeater_map;

	void perform_integrity_checks();

};  // PhatbooksDatabaseConnection


}  // namespace phatbooks


#endif  // GUARD_phatbooks_database_connection_hpp_19608494974490487

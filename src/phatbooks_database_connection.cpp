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

#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "amalgamated_budget.hpp"
#include "app.hpp"
#include "balance_cache.hpp"
#include "budget_item.hpp"
#include "commodity.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "ordinary_journal_table_iterator.hpp"
#include "repeater.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_exceptions.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using boost::numeric_cast;
using boost::optional;
using jewel::Decimal;
using jewel::Log;
using jewel::value;
using sqloxx::DatabaseConnection;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::IdentityMap;
using sqloxx::SQLStatement;
using sqloxx::SQLiteException;
using std::list;
using std::runtime_error;
using std::string;

namespace gregorian = boost::gregorian;

namespace phatbooks
{

namespace
{
	string const& setup_flag()
	{
		static string const ret = "setup_flag_996149162";
		return ret;
	}

}  // end anonymous namespace

class PhatbooksDatabaseConnection::PermanentEntityData
{
public:
	PermanentEntityData() = default;
	PermanentEntityData(PermanentEntityData const&) = delete;
	PermanentEntityData(PermanentEntityData&&) = delete;
	PermanentEntityData& operator=(PermanentEntityData const&) = delete;
	PermanentEntityData& operator=(PermanentEntityData&&) = delete;
	~PermanentEntityData() = default;
	gregorian::date creation_date() const;
	bool default_commodity_is_set() const;
	Handle<Commodity> default_commodity() const;
	
	/**
	 * @throws EntityCreationDateException if we try to set
	 * the entity creation date when it has already been
	 * initialized to some other date.
	 */
	void set_creation_date(boost::gregorian::date const& p_date);

	void set_default_commodity(Handle<Commodity> const& p_commodity);

private:
	boost::optional<boost::gregorian::date> m_creation_date;
	sqloxx::Handle<Commodity> m_default_commodity;
};

PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
	DatabaseConnection(),
	m_permanent_entity_data(nullptr),
	m_balance_cache(nullptr),
	m_budget(nullptr),
	m_account_map(nullptr),
	m_budget_item_map(nullptr),
	m_commodity_map(nullptr),
	m_entry_map(nullptr),
	m_journal_map(nullptr),
	m_repeater_map(nullptr)
{
	m_permanent_entity_data = new PermanentEntityData;
	m_balance_cache = new BalanceCache(*this);
	m_budget = new AmalgamatedBudget(*this);
	m_account_map = new IdentityMap<Account>(*this);
	m_budget_item_map = new IdentityMap<BudgetItem>(*this);
	m_commodity_map = new IdentityMap<Commodity>(*this);
	m_entry_map = new IdentityMap<Entry>(*this);
	m_journal_map = new IdentityMap<PersistentJournal>(*this);
	m_repeater_map = new IdentityMap<Repeater>(*this);
}

PhatbooksDatabaseConnection::~PhatbooksDatabaseConnection()
{
	// TODO MEDIUM PRIORITY This integrity of the objects in the cache - and the
	// avoidance of invalid reads and writes (and thus of undefined
	// behaviour) is crucially dependent on the order in which
	// the IdentityMaps are destroyed (because objects in the
	// cache contain Handles to other objects in other caches, and we
	// don't want dangling Handles).
	// This is horribly brittle! Find a way of guarding against error here.
	// Basically, where instances of a class ("Referor") contain
	// handles to instances
	// of another class ("Referee"), then the IdentityMap for
	// Referor should be
	// destroyed before the IdentityMap for Referee.
	// If I can't find a way of ensuring this automatically, then add
	// documentation to SQLoxx advising of the importance of the
	// order of deletion of the IdentityMaps.

	delete m_balance_cache;
	m_balance_cache = nullptr;

	delete m_budget;
	m_budget = nullptr;

	// Must be deleted before m_entry_map and before m_repeater_map
	delete m_journal_map;
	m_journal_map = nullptr;

	delete m_repeater_map;
	m_repeater_map = nullptr;

	// Must be deleted before m_account_map
	delete m_entry_map; 
	m_entry_map = nullptr;

	// Must be deleted before m_account_map
	delete m_budget_item_map;
	m_budget_item_map = nullptr;

	// Must be deleted before m_commodity_map
	delete m_account_map;
	m_account_map = nullptr;

	delete m_permanent_entity_data;
	m_permanent_entity_data = nullptr;

	delete m_commodity_map;
	m_commodity_map = nullptr;
}

void
PhatbooksDatabaseConnection::load_creation_date()
{
	SQLStatement statement
	(	*this,
		"select creation_date from entity_data"
	);
	statement.step();
	m_permanent_entity_data->set_creation_date
	(	boost_date_from_julian_int(statement.extract<DateRep>(0))
	);
	statement.step_final();  // Verify only one row
	return;
}

void
PhatbooksDatabaseConnection::load_default_commodity()
{
	SQLStatement statement
	(	*this,
		"select default_commodity_id from entity_data"
	);
	statement.step();
	Handle<Commodity> const commodity
	(	*this,
		statement.extract<Id>(0)
	);
	m_permanent_entity_data->set_default_commodity(commodity);
	return;
}

void
PhatbooksDatabaseConnection::do_setup()
{
	JEWEL_LOG_TRACE();
	if (!tables_are_configured())
	{
		JEWEL_ASSERT (m_permanent_entity_data);
		if (!m_permanent_entity_data->default_commodity_is_set())
		{
			// Then we create a "default default commodity".	
			Handle<Commodity> const commodity(*this);
			commodity->set_abbreviation("default commodity abbreviation");
			commodity->set_name("default commodity name");
			commodity->set_description("default commodity description");
			commodity->set_precision(2);
			commodity->set_multiplier_to_base(Decimal("1"));
			set_default_commodity(commodity);
			JEWEL_ASSERT
			(	m_permanent_entity_data->default_commodity_is_set()
			);
		}

		DatabaseTransaction transaction(*this);
		setup_boolean_table();
		Commodity::setup_tables(*this);
		setup_entity_table();
		save_default_commodity();
		Account::setup_tables(*this);
		PersistentJournal::setup_tables(*this);
		DraftJournal::setup_tables(*this);
		OrdinaryJournal::setup_tables(*this);
		Repeater::setup_tables(*this);
		BudgetItem::setup_tables(*this);
		AmalgamatedBudget::setup_tables(*this);
		Entry::setup_tables(*this);
		BalanceCache::setup_tables(*this);
		mark_tables_as_configured();
		transaction.commit();
	}
	JEWEL_ASSERT (tables_are_configured());
	load_creation_date();
	load_default_commodity();
	perform_integrity_checks();
	JEWEL_LOG_TRACE();
	return;
}

gregorian::date
PhatbooksDatabaseConnection::entity_creation_date() const
{
	return m_permanent_entity_data->creation_date();
}

gregorian::date
PhatbooksDatabaseConnection::opening_balance_journal_date() const
{
	gregorian::date const ret =
		entity_creation_date() - gregorian::date_duration(1);
	return ret;
}

void
PhatbooksDatabaseConnection::set_caching_level(unsigned int level)
{
	JEWEL_LOG_TRACE();
	switch (level)
	{
	case 0: case 1: case 2: case 3: case 4:
		m_commodity_map->disable_caching();
		m_account_map->disable_caching();
		m_budget_item_map->disable_caching();
		m_repeater_map->disable_caching();
		m_journal_map->disable_caching();
		m_entry_map->disable_caching();
		break;
	case 5: case 6: case 7: case 8: case 9:
		m_commodity_map->enable_caching();
		m_account_map->enable_caching();
		m_budget_item_map->disable_caching();
		m_repeater_map->disable_caching();
		m_journal_map->disable_caching();
		m_entry_map->disable_caching();
		break;	
	case 10: default:
		JEWEL_ASSERT (level > 0);
		m_commodity_map->enable_caching();
		m_account_map->enable_caching();
		m_budget_item_map->enable_caching();
		m_repeater_map->enable_caching();
		m_journal_map->enable_caching();
		m_entry_map->enable_caching();
		break;
	}
	JEWEL_LOG_MESSAGE(Log::info, "Caching level has been set.");
	JEWEL_LOG_VALUE(Log::info, level);
	return;
}

Handle<Account>
PhatbooksDatabaseConnection::balancing_account() const
{
	return m_budget->balancing_account();
}

Handle<Commodity>
PhatbooksDatabaseConnection::default_commodity() const
{
	return m_permanent_entity_data->default_commodity();
}

void
PhatbooksDatabaseConnection::set_default_commodity
(	Handle<Commodity> const& p_commodity
)
{
	JEWEL_ASSERT (m_permanent_entity_data);
	Handle<Commodity> const old_dc = default_commodity();
	try
	{
		m_permanent_entity_data->set_default_commodity(p_commodity);
		if (is_valid() && tables_are_configured())
		{
			save_default_commodity();  // virtual" strong guarantee
		}
	}
	catch (...)
	{
		m_permanent_entity_data->set_default_commodity(old_dc);
		throw;
	}
	return;
}

Handle<DraftJournal>
PhatbooksDatabaseConnection::budget_instrument() const
{
	return m_budget->instrument();
}

void
PhatbooksDatabaseConnection::mark_tables_as_configured()
{
	execute_sql("create table " + setup_flag() + "(dummy_column);");
	return;
}

void
PhatbooksDatabaseConnection::save_default_commodity()
{
	Handle<Commodity> const dc = default_commodity();
	DatabaseTransaction dt(*this);
	try
	{
		dc->save();
		SQLStatement statement
		(	*this,
			"update entity_data set default_commodity_id = :p"
		);
		statement.bind(":p", dc->id());
		statement.step_final();
		dt.commit();
	}
	catch (...)
	{
		if (dc->has_id()) dc->ghostify();	
		dt.cancel();
		throw;
	}
	return;
}

void
PhatbooksDatabaseConnection::setup_entity_table()
{
	// Entity table represents entity level data
	// for the database as a whole. It should only ever
	// have one row.
	SQLStatement table_creation_statement
	(	*this,
		"create table entity_data"
		"("
			"creation_date integer not null, "

			// Can be null: we want to be able to populate this
			// table before we have created any Commodities.
			"default_commodity_id references commodities"
		")"
	);
	table_creation_statement.step_final();
	SQLStatement populator
	(	*this,
		"insert into entity_data(creation_date) "
		"values(:creation_date)"
	);
	gregorian::date const today_date = today();
	populator.bind(":creation_date", julian_int(today_date));
	populator.step_final();
	m_permanent_entity_data->set_creation_date(today_date);

	return;
}

bool
PhatbooksDatabaseConnection::tables_are_configured()
{
	// TODO LOW PRIORITY Make this nicer. 
	try
	{
		execute_sql("select * from " + setup_flag());
		return true;
	}
	catch (SQLiteException&)
	{
		return false;
	}
}

Frequency
PhatbooksDatabaseConnection::budget_frequency() const
{
	return m_budget->frequency();
}

Decimal
PhatbooksDatabaseConnection::budget_balance() const
{
	return m_budget->balance();
}

bool
PhatbooksDatabaseConnection::supports_budget_frequency
(	Frequency const& p_frequency
) const
{
	return m_budget->supports_frequency(p_frequency);
}

// BalanceCacheAttorney

typedef
	PhatbooksDatabaseConnection::BalanceCacheAttorney
	BalanceCacheAttorney;


void
BalanceCacheAttorney::mark_as_stale
(	PhatbooksDatabaseConnection const& p_database_connection
)
{
	p_database_connection.m_balance_cache->mark_as_stale();
	return;
}

void
BalanceCacheAttorney::mark_as_stale
(	PhatbooksDatabaseConnection const& p_database_connection,
	sqloxx::Id p_account_id
)
{
	p_database_connection.m_balance_cache->mark_as_stale
	(	p_account_id
	);
	return;
}

Decimal
BalanceCacheAttorney::technical_balance
(	PhatbooksDatabaseConnection const& p_database_connection,
	sqloxx::Id p_account_id
)
{
	return p_database_connection.m_balance_cache->technical_balance
	(	p_account_id
	);
}

Decimal
BalanceCacheAttorney::technical_opening_balance
(	PhatbooksDatabaseConnection const& p_database_connection,
	sqloxx::Id p_account_id
)
{
	return p_database_connection.m_balance_cache->technical_opening_balance
	(	p_account_id
	);
}


// BudgetAttorney

typedef
	PhatbooksDatabaseConnection::BudgetAttorney
	BudgetAttorney;

void
BudgetAttorney::regenerate
(	PhatbooksDatabaseConnection const& p_database_connection
)
{
	p_database_connection.m_budget->regenerate();
	return;
}


Decimal
BudgetAttorney::budget
(	PhatbooksDatabaseConnection const& p_database_connection,
	sqloxx::Id p_account_id
)
{
	return p_database_connection.m_budget->budget(p_account_id);
}



// PermanentEntityData

gregorian::date
PhatbooksDatabaseConnection::PermanentEntityData::creation_date() const
{
	return value(m_creation_date);
}

bool
PhatbooksDatabaseConnection::
PermanentEntityData::default_commodity_is_set() const
{
	return static_cast<bool>(m_default_commodity);
}

Handle<Commodity>
PhatbooksDatabaseConnection::PermanentEntityData::default_commodity() const
{
	return m_default_commodity;
}

void
PhatbooksDatabaseConnection::PermanentEntityData::set_creation_date
(	boost::gregorian::date const& p_date
)
{
	if (m_creation_date && (p_date != value(m_creation_date)))
	{
		JEWEL_THROW
		(	EntityCreationDateException,
			"Entity creation date cannot be changed once set."
		);
	}
	m_creation_date = p_date;
	return;
}

void
PhatbooksDatabaseConnection::PermanentEntityData::set_default_commodity
(	Handle<Commodity> const& p_commodity
)
{
	if (p_commodity && (p_commodity->multiplier_to_base() != Decimal(1, 0)))
	{
		JEWEL_THROW
		(	InvalidDefaultCommodityException,
			"Default Commodity has multiplier_to_base that is not equal "
			"to Decimal(1, 0)."
		);
	}
	m_default_commodity = p_commodity;
	return;
}
		

// Getters for IdentityMaps

template <>
sqloxx::IdentityMap<Account>&
PhatbooksDatabaseConnection::identity_map<Account>()
{
	return *m_account_map;
}

template <>
sqloxx::IdentityMap<BudgetItem>&
PhatbooksDatabaseConnection::identity_map<BudgetItem>()
{
	return *m_budget_item_map;
}

template <>
sqloxx::IdentityMap<Entry>&
PhatbooksDatabaseConnection::identity_map<Entry>()
{
	return *m_entry_map;
}

template <>
sqloxx::IdentityMap<Commodity>&
PhatbooksDatabaseConnection::identity_map<Commodity>()
{
	return *m_commodity_map;
}

template <>
sqloxx::IdentityMap<phatbooks::PersistentJournal>&
PhatbooksDatabaseConnection::identity_map<PersistentJournal>()
{
	return *m_journal_map;
}

template <>
sqloxx::IdentityMap<phatbooks::Repeater>&
PhatbooksDatabaseConnection::identity_map<Repeater>()
{
	return *m_repeater_map;
}


void
PhatbooksDatabaseConnection::perform_integrity_checks()
{
#	ifndef NDEBUG

		// Check integrity of Account balances
		Decimal total_opening_balances;
		Decimal total_balances;
		for (AccountTableIterator it(*this), end ; it != end; ++it)
		{
			total_opening_balances += (*it)->technical_opening_balance();
			total_balances += (*it)->technical_balance();
		}
		if (total_opening_balances != Decimal(0, 0))
		{
			JEWEL_LOG_VALUE(Log::error, total_opening_balances);
		}
		if (total_balances != Decimal(0, 0))
		{
			JEWEL_LOG_VALUE(Log::error, total_balances);
		}
		JEWEL_ASSERT (total_opening_balances == Decimal(0, 0));
		JEWEL_ASSERT (total_balances == Decimal(0, 0));
		
		// Check journal dates are OK
		for (OrdinaryJournalTableIterator it(*this), end ; it != end; ++it)
		{
			JEWEL_ASSERT ((*it)->date() >= opening_balance_journal_date());
		}

#	endif
	return;
}
		


}  // namespace phatbooks





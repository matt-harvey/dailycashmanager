// Copyright (c) 2013, Matthew Harvey. All rights reserved.


/** \file phatbooks_database_connection.cpp
 *
 * \brief Source file pertaining to PhatbooksDatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "amalgamated_budget.hpp"
#include "application.hpp"
#include "budget_item.hpp"
#include "budget_item_impl.hpp"
#include "commodity_impl.hpp"
#include "date.hpp"
#include "draft_journal.hpp"
#include "draft_journal_impl.hpp"
#include "entry_impl.hpp"
#include "ordinary_journal.hpp"
#include "ordinary_journal_impl.hpp"
#include "repeater_impl.hpp"
#include "balance_cache.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "draft_journal.hpp"
#include "ordinary_journal.hpp"
#include "ordinary_journal_reader.hpp"
#include "phatbooks_exceptions.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include "repeater_reader.hpp"
#include <sqloxx/database_connection.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <cassert>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

#ifdef DEBUG
	#include <iostream>
	using std::endl;
#endif

using boost::numeric_cast;
using boost::shared_ptr;
using jewel::Decimal;
using jewel::value;
using sqloxx::DatabaseConnection;
using sqloxx::DatabaseTransaction;
using sqloxx::IdentityMap;
using sqloxx::Reader;
using sqloxx::SQLStatement;
using sqloxx::SQLiteException;
using std::endl;
using std::list;
using std::logic_error;
using std::runtime_error;
using std::string;

namespace gregorian = boost::gregorian;

namespace phatbooks
{


PhatbooksDatabaseConnection::PhatbooksDatabaseConnection():
	DatabaseConnection(),
	m_permanent_entity_data(0),
	m_balance_cache(0),
	m_budget(0),
	m_account_map(0),
	m_budget_item_map(0),
	m_commodity_map(0),
	m_entry_map(0),
	m_ordinary_journal_map(0),
	m_draft_journal_map(0),
	m_repeater_map(0)
{
	typedef PhatbooksDatabaseConnection PDC;
	m_permanent_entity_data = new PermanentEntityData;
	m_balance_cache = new BalanceCache(*this);
	m_budget = new AmalgamatedBudget(*this);
	m_account_map = new IdentityMap<AccountImpl, PDC>(*this);
	m_budget_item_map = new IdentityMap<BudgetItemImpl, PDC>(*this);
	m_commodity_map = new IdentityMap<CommodityImpl, PDC>(*this);
	m_entry_map = new IdentityMap<EntryImpl, PDC>(*this);
	m_ordinary_journal_map = new IdentityMap<OrdinaryJournalImpl, PDC>(*this);
	m_draft_journal_map = new IdentityMap<DraftJournalImpl, PDC>(*this);
	m_repeater_map = new IdentityMap<RepeaterImpl, PDC>(*this);
}

PhatbooksDatabaseConnection::~PhatbooksDatabaseConnection()
{
	// TODO This integrity of the objects in the cache - and the
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
	m_balance_cache = 0;

	delete m_budget;
	m_budget = 0;

	// Must be deleted before m_entry_map and before m_repeater_map
	delete m_draft_journal_map;
	m_draft_journal_map = 0;

	// Must be deleted before m_entry_map
	delete m_ordinary_journal_map;
	m_ordinary_journal_map = 0;

	delete m_repeater_map;
	m_repeater_map = 0;

	// Must be deleted before m_account_map
	delete m_entry_map; 
	m_entry_map = 0;

	// Must be deleted before m_account_map
	delete m_budget_item_map;
	m_budget_item_map = 0;

	// Must be deleted before m_commodity_map
	delete m_account_map;
	m_account_map = 0;

	delete m_permanent_entity_data;
	m_permanent_entity_data = 0;

	delete m_commodity_map;
	m_commodity_map = 0;
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
	Commodity commodity
	(	*this,
		statement.extract<Commodity::Id>(0)
	);
	m_permanent_entity_data->set_default_commodity(commodity);
	return;
}


void
PhatbooksDatabaseConnection::do_setup()
{
	if (!tables_are_configured())
	{
		assert (m_permanent_entity_data);
		if (!m_permanent_entity_data->default_commodity_is_set())
		{
			// Then we create a "default default commodity".	
			Commodity commodity(*this);
			commodity.set_abbreviation("default commodity abbreviation");
			commodity.set_name("default commodity name");
			commodity.set_description("default commodity description");
			commodity.set_precision(2);
			commodity.set_multiplier_to_base(Decimal("1"));
			set_default_commodity(commodity);
			assert (m_permanent_entity_data->default_commodity_is_set());
		}

		DatabaseTransaction transaction(*this);
		setup_boolean_table();
		Commodity::setup_tables(*this);
		setup_entity_table();
		save_default_commodity();
		Account::setup_tables(*this);
		ProtoJournal::setup_tables(*this);
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
	assert (tables_are_configured());
	load_creation_date();
	load_default_commodity();
	perform_integrity_checks();
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
	switch (level)
	{
	case 0: case 1: case 2: case 3: case 4:
		m_commodity_map->disable_caching();
		m_account_map->disable_caching();
		m_budget_item_map->disable_caching();
		m_repeater_map->disable_caching();
		m_draft_journal_map->disable_caching();
		m_ordinary_journal_map->disable_caching();
		m_entry_map->disable_caching();
		break;
	case 5: case 6: case 7: case 8: case 9:
		m_commodity_map->enable_caching();
		m_account_map->enable_caching();
		m_budget_item_map->disable_caching();
		m_repeater_map->disable_caching();
		m_draft_journal_map->disable_caching();
		m_ordinary_journal_map->disable_caching();
		m_entry_map->disable_caching();
		break;	
	case 10: default:
		assert (level > 0);
		m_commodity_map->enable_caching();
		m_account_map->enable_caching();
		m_budget_item_map->enable_caching();
		m_repeater_map->enable_caching();
		m_draft_journal_map->enable_caching();
		m_ordinary_journal_map->enable_caching();
		m_entry_map->enable_caching();
		break;
	}
	return;
}


Account
PhatbooksDatabaseConnection::balancing_account() const
{
	return m_budget->balancing_account();
}


Commodity
PhatbooksDatabaseConnection::default_commodity() const
{
	return m_permanent_entity_data->default_commodity();
}


void
PhatbooksDatabaseConnection::set_default_commodity
(	Commodity const& p_commodity
)
{
	assert (m_permanent_entity_data);

	// TODO Make this atomic
	m_permanent_entity_data->set_default_commodity(p_commodity);
	if (is_valid() && tables_are_configured())
	{
		save_default_commodity();
	}
	return;
}


DraftJournal
PhatbooksDatabaseConnection::budget_instrument() const
{
	return m_budget->instrument();
}


namespace
{
	string const setup_flag = "setup_flag_996149162";
}

void
PhatbooksDatabaseConnection::mark_tables_as_configured()
{
	execute_sql("create table " + setup_flag + "(dummy_column);");
	return;
}

void
PhatbooksDatabaseConnection::save_default_commodity()
{
	// TODO Make this atomic
	default_commodity().save();
	SQLStatement statement
	(	*this,
		"update entity_data set default_commodity_id = :p"
	);
	statement.bind(":p", default_commodity().id());
	statement.step_final();
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
	// TODO Make this nicer. 
	try
	{
		execute_sql("select * from " + setup_flag);
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
	AccountImpl::Id p_account_id
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
	AccountImpl::Id p_account_id
)
{
	return p_database_connection.m_balance_cache->technical_balance
	(	p_account_id
	);
}

Decimal
BalanceCacheAttorney::technical_opening_balance
(	PhatbooksDatabaseConnection const& p_database_connection,
	AccountImpl::Id p_account_id
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
	AccountImpl::Id p_account_id
)
{
	return p_database_connection.m_budget->budget(p_account_id);
}



// PermanentEntityData


PhatbooksDatabaseConnection::PermanentEntityData::PermanentEntityData():
	m_default_commodity(0)
{
}

PhatbooksDatabaseConnection::PermanentEntityData::~PermanentEntityData()
{
	delete m_default_commodity;
	m_default_commodity = 0;
}

gregorian::date
PhatbooksDatabaseConnection::PermanentEntityData::creation_date() const
{
	return value(m_creation_date);
}

bool
PhatbooksDatabaseConnection::
PermanentEntityData::default_commodity_is_set() const
{
	return m_default_commodity != 0;
}

Commodity
PhatbooksDatabaseConnection::PermanentEntityData::default_commodity() const
{
	if (!default_commodity_is_set())
	{
		throw std::logic_error("Default commodity has not been set.");
	}
	assert (m_default_commodity);
	return *m_default_commodity;
}

void
PhatbooksDatabaseConnection::PermanentEntityData::set_creation_date
(	boost::gregorian::date const& p_date
)
{
	if (m_creation_date && (p_date != value(m_creation_date)))
	{
		throw EntityCreationDateException
		(	"Entity creation date cannot be changed once set."
		);
	}
	m_creation_date = p_date;
	return;
}

void
PhatbooksDatabaseConnection::PermanentEntityData::set_default_commodity
(	Commodity const& p_commodity
)
{
	if (p_commodity.multiplier_to_base() != Decimal(1, 0))
	{
		throw InvalidDefaultCommodityException
		(	"Default Commodity has multiplier_to_base that is not equal "
			"to Decimal(1, 0)."
		);
	}
	if (m_default_commodity) *m_default_commodity = p_commodity;
	else m_default_commodity = new Commodity(p_commodity);
	return;
}
		

// Getters for IdentityMaps

template <>
sqloxx::IdentityMap<AccountImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<AccountImpl>()
{
	return *m_account_map;
}

template <>
sqloxx::IdentityMap<BudgetItemImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<BudgetItemImpl>()
{
	return *m_budget_item_map;
}

template <>
sqloxx::IdentityMap<EntryImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<EntryImpl>()
{
	return *m_entry_map;
}

template <>
sqloxx::IdentityMap<CommodityImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<CommodityImpl>()
{
	return *m_commodity_map;
}

template <>
sqloxx::IdentityMap<OrdinaryJournalImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<OrdinaryJournalImpl>()
{
	return *m_ordinary_journal_map;
}

template <>
sqloxx::IdentityMap<phatbooks::DraftJournalImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<DraftJournalImpl>()
{
	return *m_draft_journal_map;
}

template <>
sqloxx::IdentityMap<phatbooks::RepeaterImpl, PhatbooksDatabaseConnection>&
PhatbooksDatabaseConnection::identity_map<RepeaterImpl>()
{
	return *m_repeater_map;
}


void
PhatbooksDatabaseConnection::perform_integrity_checks()
{
#	ifndef NDEBUG

		// Check integrity of Account balances
		AccountReader const account_reader(*this);
		Decimal total_opening_balances;
		Decimal total_balances;
		for
		(	AccountReader::const_iterator it = account_reader.begin();
			it != account_reader.end();
			++it
		)
		{
			total_opening_balances += it->technical_opening_balance();
			total_balances += it->technical_balance();
		}
		assert (total_opening_balances == Decimal(0, 0));
		assert (total_balances == Decimal(0, 0));
		
		// Check journal dates are OK
		OrdinaryJournalReader const oj_reader(*this);
		for
		(	OrdinaryJournalReader::const_iterator it = oj_reader.begin();
			it != oj_reader.end();
			++it
		)
		{
			assert (it->date() >= opening_balance_journal_date());
		}

#	endif
	return;
}
		


}  // namespace phatbooks





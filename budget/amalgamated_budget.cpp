#include "amalgamated_budget.hpp"
#include "account.hpp"
#include "account_impl.hpp"
#include "budget_item_reader.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "repeater.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/static_assert.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/sql_statement.hpp>
#include <algorithm>
#include <cassert>
#include <vector>

using boost::scoped_ptr;
using jewel::Decimal;
using sqloxx::SQLStatement;
using std::vector;

namespace gregorian = boost::gregorian;

// For debugging only
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff




namespace phatbooks
{


BOOST_STATIC_ASSERT((boost::is_same<Account::Id, AccountImpl::Id>::value));


void
AmalgamatedBudget::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	JEWEL_DEBUG_LOG << "Setting up AmalgamatedBudget tables." << endl;
	dbc.execute_sql
	(	"create index budget_item_account_index on budget_items(account_id)"
	);
	dbc.execute_sql
	(	"create table amalgamated_budget_data(journal_id integer primary key "
		"references draft_journal_detail)"
	);
	DraftJournal instrument(dbc);
	instrument.set_name("AMALGAMATED BUDGET INSTRUMENT");
	instrument.set_whether_actual(false);
	instrument.set_comment("");
	instrument.save();
	SQLStatement statement
	(	dbc,
		"insert into amalgamated_budget_data(journal_id) values(:p)"
	);
	statement.bind(":p", instrument.id());
	statement.step_final();
	JEWEL_DEBUG_LOG << "AmalgamatedBudget tables have been set up." << endl;
	return;
}

AmalgamatedBudget::AmalgamatedBudget
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection),
	m_frequency(1, interval_type::days),
	m_map(new Map),
	m_map_is_stale(true),
	m_instrument(0)
{
}


AmalgamatedBudget::~AmalgamatedBudget()
{
	delete m_instrument;
	m_instrument = 0;
}


Frequency
AmalgamatedBudget::frequency() const
{
	assert (m_frequency.num_steps() == 1);
	return m_frequency;
}


void
AmalgamatedBudget::set_frequency(Frequency const& p_frequency)
{
	mark_as_stale();
	m_frequency = p_frequency;
	return;
}



Decimal
AmalgamatedBudget::budget(AccountImpl::Id p_account_id)
{
	if (m_map_is_stale)
	{
		refresh();
	}
	Map::const_iterator it = m_map->find(p_account_id);
	assert (it != m_map->end());
	return it->second;
}


// TODO Make sure I mark as stale whenever required.
void
AmalgamatedBudget::mark_as_stale()
{
	m_map_is_stale = true;
	// TODO When a BudgetItem is created, changed or etc.., we need a way
	// to reflect this \e immediately in the "instrument" DraftJournal.
}

void
AmalgamatedBudget::generate_supported_frequencies(vector<Frequency>& vec)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supports_frequency. If this
	// changes, that must change too.
	vec.push_back(Frequency(1, interval_type::days));
	vec.push_back(Frequency(1, interval_type::weeks));
	vec.push_back(Frequency(2, interval_type::weeks));
	vec.push_back(Frequency(4, interval_type::weeks));
	vec.push_back(Frequency(1, interval_type::months));
	vec.push_back(Frequency(2, interval_type::months));
	vec.push_back(Frequency(3, interval_type::months));
	vec.push_back(Frequency(4, interval_type::months));
	vec.push_back(Frequency(6, interval_type::months));
	vec.push_back(Frequency(12, interval_type::months));
	return;
}


bool
AmalgamatedBudget::supports_frequency(Frequency const& p_frequency)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supported_frequencies. If this changes,
	// that must change too.
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_frequency.num_steps() == 1;
	case interval_type::weeks:
		switch (p_frequency.num_steps())
		{
		case 1: case 2:	return true;
		case 3:			return false;
		case 4:			return true;
		default: 		return false;
		}
	case interval_type::months:
		switch (p_frequency.num_steps())
		{
		case 1: case 2: case 3: case 4:				return true;
		case 5:										return false;
		case 6:										return true;
		case 7: case 8: case 9: case 10: case 11:	return false;
		case 12:									return true;	
		default:									return false;
		}
	case interval_type::month_ends:
		return false;
	default:
		assert (false);
	}
}
		
void
AmalgamatedBudget::refresh()
{
	JEWEL_DEBUG_LOG << "Refreshing AmalgamatedBudget." << endl;
	scoped_ptr<Map> map_elect(new Map);
	assert (map_elect->empty());

	SQLStatement account_selector
	(	m_database_connection,
		"select account_id from accounts"
	);
	while (account_selector.step())
	{
		AccountImpl::Id const account_id =
			account_selector.extract<AccountImpl::Id>(0);
		Account const account(m_database_connection, account_id);
		(*map_elect)[account_id] =
			Decimal(0, account.commodity().precision());
	}
	BudgetItemReader budget_item_reader(m_database_connection);
	BudgetItemReader::const_iterator const beg = budget_item_reader.begin();
	BudgetItemReader::const_iterator const end = budget_item_reader.end();
	
	JEWEL_DEBUG_LOG << "Calculating AmalgamatedBudget using canonical frequency." << endl;
	// First we calculate budgets amalgamated on the basis of
	// the canonical frequency
	BudgetItemReader::const_iterator it = beg;
	for ( ; it != end; ++it)
	{
		Frequency const raw_frequency = it->frequency();
		if (!AmalgamatedBudget::supports_frequency(raw_frequency))
		{
			throw InvalidFrequencyException
			(	"Frequency not supported by AmalgamatedBudget."
			);
		}
		AccountImpl::Id const account_id = it->account().id();
		jewel::Decimal const raw_amount = it->amount();
		jewel::Decimal const canonical_amount = convert_to_canonical
		(	raw_frequency,
			raw_amount
		);
		Map::iterator tmit = map_elect->find(account_id);
		assert (tmit != map_elect->end());	
		tmit->second += canonical_amount;
	}
	JEWEL_DEBUG_LOG << "Converting AmalgamatedBudget to target frequency." << endl;
	// Now convert to desired frequency
	for 
	(	Map::iterator mit = map_elect->begin(), mend = map_elect->end();
		mit != mend;
		++mit
	)
	{
		mit->second = convert_from_canonical(m_frequency, mit->second);
	}
	load_instrument();
	refresh_instrument();
	using std::swap;
	swap(m_map, map_elect);
	m_map_is_stale = false;
	return;
}


void
AmalgamatedBudget::refresh_instrument()
{
	JEWEL_DEBUG_LOG << "Refreshing AmalgamatedBudget::m_instrument." << endl;
	DraftJournal fresh_journal(m_database_connection);
	JEWEL_DEBUG_LOG << "Temp journal is mimicking existing m_instrument." << endl;
	fresh_journal.mimic(*m_instrument);
	JEWEL_DEBUG_LOG << "Reflecting correct entries into temp journal." << endl;
	reflect_entries(fresh_journal);
	JEWEL_DEBUG_LOG << "Reflecting correct repeater into temp journal." << endl;
	reflect_repeater(fresh_journal);
	JEWEL_DEBUG_LOG << "Copying temp across to m_instrument." << endl;
	(*m_instrument) = fresh_journal;
	JEWEL_DEBUG_LOG << "Instrument now looks like this:\n" << *m_instrument << endl;
	return;
}


void
AmalgamatedBudget::load_instrument()
{
	// Set the instrument (the DraftJournal that carries out
	// the AmalgamatedBudget)
	SQLStatement statement
	(	m_database_connection,
		"select journal_id from amalgamated_budget_data"
	);
	statement.step();
	if (m_instrument)
	{
		delete m_instrument;
		m_instrument = 0;
	}
	m_instrument = new DraftJournal
	(	m_database_connection,
		statement.extract<DraftJournal::Id>(0)
	);
	statement.step_final();
	return;
}


void
AmalgamatedBudget::reflect_entries(DraftJournal& journal)
{
	journal.clear_entries();
	Map const& map = *m_map;
	for
	(	Map::const_iterator it = map.begin(), end = map.end();
		it != end;
		++it
	)
	{
		Entry entry(m_database_connection);
		Account const account(m_database_connection, it->first);
		entry.set_account(account);
		entry.set_comment("");
		entry.set_amount(it->second);
		entry.set_whether_reconciled(false);
		journal.push_entry(entry);
	}
	return;
}


void
AmalgamatedBudget::reflect_repeater(DraftJournal& journal)
{
	vector<Repeater> const& old_repeaters = journal.repeaters();
	if (old_repeaters.size() == 1)
	{
		Frequency const old_frequency = old_repeaters[0].frequency();
		if 
		(	old_frequency.step_type() == m_frequency.step_type() &&
			old_frequency.num_steps() == m_frequency.num_steps()
		)
		{
			return;
		}
	}
	journal.clear_repeaters();
	Repeater new_repeater(m_database_connection);
	new_repeater.set_frequency(m_frequency);
	new_repeater.set_next_date(gregorian::day_clock::local_day());
	assert (journal.repeaters().empty());
	journal.push_repeater(new_repeater);
	return;
}



}  // namespace phatbooks



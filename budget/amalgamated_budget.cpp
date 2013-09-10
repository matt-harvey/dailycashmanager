// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "amalgamated_budget.hpp"
#include "account.hpp"
#include "account_impl.hpp"
#include "account_type.hpp"
#include "budget_item_table_iterator.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "transaction_type.hpp"
#include "visibility.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/static_assert.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/sql_statement.hpp>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <utility>
#include <vector>

using boost::scoped_ptr;
using jewel::Decimal;
using sqloxx::SQLStatement;
using std::accumulate;
using std::ostream;
using std::pair;
using std::vector;

namespace gregorian = boost::gregorian;

// For debugging only
#include <jewel/log.hpp>
#include <iostream>
using std::endl;
// End debugging stuff




namespace phatbooks
{


BOOST_STATIC_ASSERT((boost::is_same<Account::Id, AccountImpl::Id>::value));


namespace
{
	// Identifies which (if any) is the balancing Entry, in the instrument.
	wxString balancing_entry_comment()
	{
		return wxString("Balancing entry");
	}

}  // end anonymous namespace

void
AmalgamatedBudget::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create index budget_item_account_index on budget_items(account_id)"
	);
	// NOTE UserDraftJournalReader has knowledge of this schema. Ensure
	// any changes to the schema are also reflected there, as appropriate.
	dbc.execute_sql
	(	"create table amalgamated_budget_data"
		"("
			"journal_id integer unique not null "
			"references draft_journal_detail, "
			"balancing_account_id integer unique not null "
			"references accounts"
		")"
	);
	DraftJournal instrument(dbc);
	instrument.set_name("AMALGAMATED BUDGET JOURNAL");
	instrument.set_comment("");
	instrument.set_transaction_type(transaction_type::envelope_transaction);
	Repeater repeater(dbc);
	repeater.set_frequency(Frequency(1, interval_type::days));
	repeater.set_next_date(gregorian::day_clock::local_day());
	instrument.push_repeater(repeater);
	instrument.save();

	Account balancing_account(dbc);
	balancing_account.set_account_type(account_type::pure_envelope);
	balancing_account.set_name("Budget imbalance");
	balancing_account.set_description("");
	balancing_account.set_visibility(visibility::visible);
	Commodity const balancing_account_commodity = dbc.default_commodity();
	balancing_account.set_commodity(balancing_account_commodity);
	balancing_account.save();

	SQLStatement statement
	(	dbc,
		"insert into amalgamated_budget_data"
		"(journal_id, balancing_account_id) "
		"values(:journal_id, :balancing_account_id)"
	);
	statement.bind(":journal_id", instrument.id());
	statement.bind(":balancing_account_id", balancing_account.id());
	statement.step_final();

	return;
}

AmalgamatedBudget::AmalgamatedBudget
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_is_loaded(false),
	m_database_connection(p_database_connection),
	m_frequency(1, interval_type::days),
	m_map(new Map),
	m_instrument(0),
	m_balancing_account(0)
{
}


AmalgamatedBudget::~AmalgamatedBudget()
{
	delete m_instrument;
	m_instrument = 0;

	delete m_balancing_account;
	m_balancing_account = 0;
}


void
AmalgamatedBudget::load() const
{
	if (m_is_loaded)
	{
		return;
	}
	JEWEL_ASSERT (!m_is_loaded);
	load_balancing_account();
	load_instrument();
	load_map();
	m_is_loaded = true;
	return;
}


Frequency
AmalgamatedBudget::frequency() const
{
	load();
	JEWEL_ASSERT (m_frequency.num_steps() == 1);
	return m_frequency;
}

void
AmalgamatedBudget::set_frequency(Frequency const& p_frequency)
{
	load();
	m_frequency = p_frequency;
	regenerate();
	return;
}

Decimal
AmalgamatedBudget::budget(AccountImpl::Id p_account_id) const
{
	load();
	Map::const_iterator it = m_map->find(p_account_id);
	JEWEL_ASSERT (it != m_map->end());
	Decimal ret = it->second;
	if (p_account_id == balancing_account().id())
	{
		ret += instrument_balancing_amount();	
	}
	return ret;
}


namespace
{
	Decimal map_entry_accumulation_aux
	(	Decimal const& dec,
		pair<AccountImpl::Id, Decimal> const& rhs
	)
	{
		return dec + rhs.second;
	}

}  // end anonymous namespace	


Decimal
AmalgamatedBudget::balance() const
{
	load();
	return accumulate
	(	m_map->begin(),
		m_map->end(),
		Decimal(0, 0),
		map_entry_accumulation_aux
	);
}	

void
AmalgamatedBudget::generate_supported_frequencies(vector<Frequency>& vec)
{
	// NOTE This is co-dependent with the function
	// AmalgamatedBudget::supports_frequency. If this
	// changes, that must change too.
	vec.reserve(10);
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
		JEWEL_HARD_ASSERT (false);
	}
}

Account
AmalgamatedBudget::balancing_account() const
{
	load();
	JEWEL_ASSERT (m_balancing_account != 0);
	return *m_balancing_account;
}

DraftJournal
AmalgamatedBudget::instrument() const
{
	load();
	return *m_instrument;
}

void
AmalgamatedBudget::regenerate()
{
	// If amalgamated_budget_data has not yet been populated, then
	// proceeding here would cause the program to crash, as
	// we wouldn't be able to load the balancing account id, etc..
	// So if amalgamated_budget_data has not been created yet,
	// we simply return. It is expected this will only happen on
	// initial setup of the database (due to calling of
	// regenerate() by hook in AccountImpl saving method, when
	// balancing account is first saved).
	SQLStatement statement
	(	m_database_connection,
		"select * from amalgamated_budget_data"
	);
	if (statement.step())
	{
		load();
		regenerate_map();
		regenerate_instrument();
		statement.step_final();
	}
	return;
}

void
AmalgamatedBudget::load_map() const
{
	JEWEL_ASSERT (!m_is_loaded);
	JEWEL_ASSERT (m_map->empty());
	generate_map();
	return;
}


void
AmalgamatedBudget::generate_map() const
{
	scoped_ptr<Map> map_elect(new Map);
	JEWEL_ASSERT (map_elect->empty());

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
	
	// First we calculate budgets amalgamated on the basis of
	// the canonical frequency
	BudgetItemTableIterator it(m_database_connection);
	BudgetItemTableIterator const end;
	for ( ; it != end; ++it)
	{
		Frequency const raw_frequency = it->frequency();
		if (!AmalgamatedBudget::supports_frequency(raw_frequency))
		{
			JEWEL_THROW
			(	InvalidFrequencyException,
				"Frequency not supported by AmalgamatedBudget."
			);
		}
		AccountImpl::Id const account_id = it->account().id();
		jewel::Decimal const raw_amount = it->amount();
		jewel::Decimal const canonical_amount = convert_to_canonical
		(	raw_frequency,
			raw_amount
		);
		Map::iterator tmit = map_elect->find(account_id);
		JEWEL_ASSERT (tmit != map_elect->end());	
		tmit->second += canonical_amount;
	}
	// Now convert to desired frequency
	for 
	(	Map::iterator mit = map_elect->begin(), mend = map_elect->end();
		mit != mend;
		++mit
	)
	{
		mit->second = round
		(	convert_from_canonical(m_frequency, mit->second),
			Account(m_database_connection, mit->first).commodity().precision()
		);
	}
	using std::swap;
	swap(m_map, map_elect);

	return;
}


void
AmalgamatedBudget::regenerate_map()
{
	load();
	generate_map();
	return;
}


void
AmalgamatedBudget::regenerate_instrument()
{
	load();

	DraftJournal fresh_journal(m_database_connection);
	fresh_journal.mimic(*m_instrument);
	reflect_entries(fresh_journal);
	reflect_repeater(fresh_journal);

	// Deal with imbalance
	Decimal const imbalance = fresh_journal.balance();
	if (imbalance != Decimal(0, 0))
	{
		Account const ba = balancing_account();
		Entry balancing_entry(m_database_connection);
		balancing_entry.set_account(ba);
		balancing_entry.set_comment(balancing_entry_comment());
		balancing_entry.set_whether_reconciled(false);
		balancing_entry.set_amount
		(	-round(imbalance, ba.commodity().precision())
		);
		balancing_entry.set_transaction_side(transaction_side::destination);
		fresh_journal.push_entry(balancing_entry);
		JEWEL_ASSERT (fresh_journal.is_balanced());
	}
	// WARNING The source and destination are the opposite way
	// round to usual here. But it probably doesn't matter, as
	// the user won't be seeing this Journal anyway.
	m_instrument->mimic(fresh_journal);
	m_instrument->save();

	return;
}

void
AmalgamatedBudget::load_balancing_account() const
{
	SQLStatement statement
	(	m_database_connection,
		"select balancing_account_id from amalgamated_budget_data"
	);
	bool check = statement.step();
	JEWEL_ASSERT (check);
	if (m_balancing_account)
	{
		delete m_balancing_account;
		m_balancing_account = 0;
	}
	m_balancing_account = new Account
	(	m_database_connection,
		statement.extract<Account::Id>(0)
	);
	statement.step_final();
	return;
}

void
AmalgamatedBudget::load_instrument() const
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
AmalgamatedBudget::reflect_entries(DraftJournal& p_journal)
{
	load();
	p_journal.clear_entries();
	Map const& map = *m_map;
	for
	(	Map::const_iterator it = map.begin(), end = map.end();
		it != end;
		++it
	)
	{
		if (it->second != Decimal(0, 0))
		{
			Entry entry(m_database_connection);
			entry.set_account
			(	Account(m_database_connection, it->first)
			);
			entry.set_comment("");
			entry.set_amount(-(it->second));
			entry.set_whether_reconciled(false);
			entry.set_transaction_side(transaction_side::source);
			p_journal.push_entry(entry);
		}
	}
	return;
}

void
AmalgamatedBudget::reflect_repeater(DraftJournal& p_journal)
{
	load();
	vector<Repeater> const& old_repeaters = p_journal.repeaters();
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
	p_journal.clear_repeaters();
	Repeater new_repeater(m_database_connection);
	new_repeater.set_frequency(m_frequency);
	new_repeater.set_next_date(gregorian::day_clock::local_day());
	JEWEL_ASSERT (p_journal.repeaters().empty());
	p_journal.push_repeater(new_repeater);
	return;
}

Decimal
AmalgamatedBudget::instrument_balancing_amount() const
{
	load();
	JEWEL_ASSERT (m_instrument);
	vector<Entry> const& entries = m_instrument->entries();
	Decimal ret(0, m_database_connection.default_commodity().precision());
	vector<Entry>::const_iterator it = entries.begin();
	vector<Entry>::const_iterator const end = entries.end();
	wxString const balancing_entry_marker = balancing_entry_comment();
	for ( ; it != end; ++it)
	{
		if (it->comment() == balancing_entry_marker)
		{
			ret -= it->amount();
		}
	}

#	ifndef NDEBUG
	if (!entries.empty())
	{
		JEWEL_ASSERT (m_instrument->repeaters().size() == 1);
		Repeater const repeater = m_instrument->repeaters()[0];
		JEWEL_ASSERT (repeater.frequency() == frequency());
	}
#	endif

	return ret;
}


}  // namespace phatbooks



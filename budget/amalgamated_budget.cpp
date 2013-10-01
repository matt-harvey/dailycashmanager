// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "amalgamated_budget.hpp"
#include "account_handle.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "budget_item_table_iterator.hpp"
#include "commodity_handle.hpp"
#include "draft_journal_handle.hpp"
#include "entry_handle.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater_handle.hpp"
#include "transaction_type.hpp"
#include "visibility.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/sql_statement.hpp>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <utility>
#include <vector>

using jewel::Decimal;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::accumulate;
using std::ostream;
using std::pair;
using std::unique_ptr;
using std::vector;

namespace gregorian = boost::gregorian;


namespace phatbooks
{



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
	// NOTE UserDraftJournalTableIterator has knowledge of this schema. Ensure
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
	DraftJournalHandle const instrument(dbc);
	instrument->set_name("AMALGAMATED BUDGET JOURNAL");
	instrument->set_comment("");
	instrument->set_transaction_type(TransactionType::envelope);
	RepeaterHandle const repeater(dbc);
	repeater->set_frequency(Frequency(1, IntervalType::days));
	repeater->set_next_date(gregorian::day_clock::local_day());
	instrument->push_repeater(repeater);
	instrument->save();

	AccountHandle balancing_account(dbc);
	balancing_account->set_account_type(AccountType::pure_envelope);
	balancing_account->set_name("Budget imbalance");
	balancing_account->set_description("");
	balancing_account->set_visibility(Visibility::visible);
	CommodityHandle const balancing_account_commodity =
		dbc.default_commodity();
	balancing_account->set_commodity(balancing_account_commodity);
	balancing_account->save();

	SQLStatement statement
	(	dbc,
		"insert into amalgamated_budget_data"
		"(journal_id, balancing_account_id) "
		"values(:journal_id, :balancing_account_id)"
	);
	statement.bind(":journal_id", instrument->id());
	statement.bind(":balancing_account_id", balancing_account->id());
	statement.step_final();

	return;
}

AmalgamatedBudget::AmalgamatedBudget
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_is_loaded(false),
	m_database_connection(p_database_connection),
	m_frequency(1, IntervalType::days),
	m_map(new Map),
	m_instrument(nullptr),
	m_balancing_account(nullptr)
{
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
AmalgamatedBudget::budget(sqloxx::Id p_account_id) const
{
	load();
	Map::const_iterator it = m_map->find(p_account_id);
	JEWEL_ASSERT (it != m_map->end());
	Decimal ret = it->second;
	if (p_account_id == balancing_account()->id())
	{
		ret += instrument_balancing_amount();	
	}
	return ret;
}


namespace
{
	Decimal map_entry_accumulation_aux
	(	Decimal const& dec,
		pair<Id, Decimal> const& rhs
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
	vec.push_back(Frequency(1, IntervalType::days));
	vec.push_back(Frequency(1, IntervalType::weeks));
	vec.push_back(Frequency(2, IntervalType::weeks));
	vec.push_back(Frequency(4, IntervalType::weeks));
	vec.push_back(Frequency(1, IntervalType::months));
	vec.push_back(Frequency(2, IntervalType::months));
	vec.push_back(Frequency(3, IntervalType::months));
	vec.push_back(Frequency(4, IntervalType::months));
	vec.push_back(Frequency(6, IntervalType::months));
	vec.push_back(Frequency(12, IntervalType::months));
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
	case IntervalType::days:
		return p_frequency.num_steps() == 1;
	case IntervalType::weeks:
		switch (p_frequency.num_steps())
		{
		case 1: case 2:	return true;
		case 3:			return false;
		case 4:			return true;
		default: 		return false;
		}
	case IntervalType::months:
		switch (p_frequency.num_steps())
		{
		case 1: case 2: case 3: case 4:				return true;
		case 5:										return false;
		case 6:										return true;
		case 7: case 8: case 9: case 10: case 11:	return false;
		case 12:									return true;	
		default:									return false;
		}
	case IntervalType::month_ends:
		return false;
	default:
		JEWEL_HARD_ASSERT (false);
	}
}

AccountHandle
AmalgamatedBudget::balancing_account() const
{
	load();
	JEWEL_ASSERT (m_balancing_account != 0);
	return *m_balancing_account;
}

DraftJournalHandle
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
	// regenerate() by hook in Account saving method, when
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
	unique_ptr<Map> map_elect(new Map);
	JEWEL_ASSERT (map_elect->empty());

	SQLStatement account_selector
	(	m_database_connection,
		"select account_id from accounts"
	);
	while (account_selector.step())
	{
		Id const account_id = account_selector.extract<Id>(0);
		AccountHandle const account(m_database_connection, account_id);
		(*map_elect)[account_id] =
			Decimal(0, account->commodity()->precision());
	}
	
	// First we calculate budgets amalgamated on the basis of
	// the canonical frequency
	BudgetItemTableIterator it(m_database_connection);
	BudgetItemTableIterator const end;
	for ( ; it != end; ++it)
	{
		BudgetItemHandle const& budget_item = *it;
		Frequency const raw_frequency = budget_item->frequency();
		if (!AmalgamatedBudget::supports_frequency(raw_frequency))
		{
			JEWEL_THROW
			(	InvalidFrequencyException,
				"Frequency not supported by AmalgamatedBudget."
			);
		}
		Id const account_id = budget_item->account()->id();
		jewel::Decimal const raw_amount = budget_item->amount();
		jewel::Decimal const canonical_amount = convert_to_canonical
		(	raw_frequency,
			raw_amount
		);
		Map::iterator tmit = map_elect->find(account_id);
		JEWEL_ASSERT (tmit != map_elect->end());	
		tmit->second += canonical_amount;
	}
	// Now convert to desired frequency
	for (auto& slot: *map_elect)
	{
		slot.second = round
		(	convert_from_canonical(m_frequency, slot.second),
			AccountHandle(m_database_connection, slot.first)->
				commodity()->precision()
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

	DraftJournalHandle const fresh_journal(m_database_connection);
	fresh_journal->mimic(**m_instrument);
	reflect_entries(fresh_journal);
	reflect_repeater(fresh_journal);

	// Deal with imbalance
	Decimal const imbalance = fresh_journal->balance();
	if (imbalance != Decimal(0, 0))
	{
		AccountHandle const ba = balancing_account();
		EntryHandle const balancing_entry(m_database_connection);
		balancing_entry->set_account(ba);
		balancing_entry->set_comment(balancing_entry_comment());
		balancing_entry->set_whether_reconciled(false);
		balancing_entry->set_amount
		(	-round(imbalance, ba->commodity()->precision())
		);
		balancing_entry->set_transaction_side(TransactionSide::destination);
		fresh_journal->push_entry(balancing_entry);
		JEWEL_ASSERT (fresh_journal->is_balanced());
	}
	// WARNING The source and destination are the opposite way
	// round to usual here. But it probably doesn't matter, as
	// the user won't be seeing this Journal anyway.
	(*m_instrument)->mimic(*fresh_journal);
	(*m_instrument)->save();

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
	m_balancing_account.reset
	(	new AccountHandle
		(	m_database_connection,
			statement.extract<Id>(0)
		)
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
	m_instrument.reset
	(	new DraftJournalHandle
		(	m_database_connection,
			statement.extract<Id>(0)
		)
	);
	statement.step_final();
	return;
}


void
AmalgamatedBudget::reflect_entries(DraftJournalHandle const& p_journal)
{
	load();
	p_journal->clear_entries();
	for (auto const& elem: *m_map)
	{
		if (elem.second != Decimal(0, 0))
		{
			EntryHandle const entry(m_database_connection);
			entry->set_account
			(	AccountHandle(m_database_connection, elem.first)
			);
			entry->set_comment("");
			entry->set_amount(-(elem.second));
			entry->set_whether_reconciled(false);
			entry->set_transaction_side(TransactionSide::source);
			p_journal->push_entry(entry);
		}
	}
	return;
}

void
AmalgamatedBudget::reflect_repeater(DraftJournalHandle const& p_journal)
{
	load();
	vector<RepeaterHandle> const& old_repeaters = p_journal->repeaters();
	if (old_repeaters.size() == 1)
	{
		Frequency const old_frequency = old_repeaters[0]->frequency();
		if 
		(	old_frequency.step_type() == m_frequency.step_type() &&
			old_frequency.num_steps() == m_frequency.num_steps()
		)
		{
			return;
		}
	}
	p_journal->clear_repeaters();
	RepeaterHandle const new_repeater(m_database_connection);
	new_repeater->set_frequency(m_frequency);
	new_repeater->set_next_date(gregorian::day_clock::local_day());
	JEWEL_ASSERT (p_journal->repeaters().empty());
	p_journal->push_repeater(new_repeater);
	return;
}

Decimal
AmalgamatedBudget::instrument_balancing_amount() const
{
	load();
	JEWEL_ASSERT (m_instrument);
	Decimal ret(0, m_database_connection.default_commodity()->precision());
	wxString const balancing_entry_marker = balancing_entry_comment();
	vector<EntryHandle> const& entries = (*m_instrument)->entries();
	for (EntryHandle const& entry: entries)
	{
		if (entry->comment() == balancing_entry_marker)
		{
			ret -= entry->amount();
		}
	}

#	ifndef NDEBUG
	if (!entries.empty())
	{
		JEWEL_ASSERT ((*m_instrument)->repeaters().size() == 1);
		RepeaterHandle const repeater = (*m_instrument)->repeaters()[0];
		JEWEL_ASSERT (repeater->frequency() == frequency());
	}
#	endif

	return ret;
}


}  // namespace phatbooks



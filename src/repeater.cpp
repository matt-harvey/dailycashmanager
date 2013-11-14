/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "repeater.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "frequency.hpp"
#include "draft_journal.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "proto_journal.hpp"
#include "repeater.hpp"
#include "repeater_table_iterator.hpp"
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/checked_arithmetic.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace gregorian = boost::gregorian;

using boost::optional;
using boost::numeric_cast;
using jewel::clear;
using jewel::multiplication_is_unsafe;
using jewel::value;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::is_same;
using std::move;
using std::numeric_limits;
using std::ostringstream;
using std::sort;
using std::string;
using std::vector;

namespace phatbooks
{

struct Repeater::RepeaterData
{
	optional<Frequency> frequency;
	optional<DateRep> next_date;
	optional<Id> journal_id;
};

void
Repeater::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table interval_types"
		"("
			"interval_type_id integer primary key"
		");"
		"insert into interval_types(interval_type_id) values(1); "
		"insert into interval_types(interval_type_id) values(2); "
		"insert into interval_types(interval_type_id) values(3); "
		"insert into interval_types(interval_type_id) values(4);"
	);
	dbc.execute_sql
	(	"create table repeaters"
		"("
			"repeater_id integer primary key autoincrement, "
			"interval_type_id integer not null references interval_types, "
			"interval_units integer not null, "
			"next_date integer not null, "
			"journal_id integer not null references draft_journal_detail "
		");"
	);
	return;
}

Repeater::Repeater
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map),
	m_data(new RepeaterData)
{
	(void)p_signature;  // silence compiler re. unused parameter
}

Repeater::Repeater
(	IdentityMap& p_identity_map,	
	Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map, p_id),
	m_data(new RepeaterData)
{
	(void)p_signature;  // silence compiler re. unused parameter
}

Repeater::~Repeater() = default;

void
Repeater::set_frequency(Frequency const& p_frequency)
{
	load();
	if (m_data->next_date)
	{
		auto const next =
			boost_date_from_julian_int(*(m_data->next_date));
		auto const interval = p_frequency.step_type();
		if (!is_valid_date_for_interval_type(next, interval))
		{
			ostringstream oss;
			oss << "Attempted to set Repeater Frequency to a Frequency "
				<< "with a step_type() of "
				<< "\""
				<< interval
				<< "\", however this is incompatible with the Repeater "
				<< "next_date(), which has been set to "
				<< next
				<< ".";
			char const* msg = oss.str().c_str();
			JEWEL_THROW (InvalidFrequencyException, msg);
		}
	}
	m_data->frequency = p_frequency;
	return;
}

void
Repeater::set_next_date(boost::gregorian::date const& p_next_date)
{
	if (p_next_date < database_connection().entity_creation_date())
	{
		JEWEL_THROW
		(	InvalidRepeaterDateException,
			"Next firing date of Repeater cannot be set to a date "
			"earlier than the entity creation date."
		);
	}
	JEWEL_ASSERT
	(	p_next_date >=
		database_connection().entity_creation_date()
	);
	load();
	if (m_data->frequency)
	{
		IntervalType const interval = m_data->frequency->step_type();
		if (!is_valid_date_for_interval_type(p_next_date, interval))
		{
			ostringstream oss;
			oss << "Attempted to set Repeater next_date() to "
				<< p_next_date
				<< ", however this is incompatible with the Frequency "
				<< "that has already been set for the Repeater and has "
				<< "a step_type() of \""
				<< interval
				<< "\".";
			char const* msg = oss.str().c_str();
			JEWEL_THROW (InvalidRepeaterDateException, msg);
		}
	}
	m_data->next_date = julian_int(p_next_date);
	return;
}

void
Repeater::set_journal_id(Id p_journal_id)
{
	load();
	m_data->journal_id = p_journal_id;
	return;
}


Frequency
Repeater::frequency()
{
	load();
	return value(m_data->frequency);
}


gregorian::date
Repeater::next_date(vector<gregorian::date>::size_type n)
{
	load();
	typedef vector<gregorian::date>::size_type Size;
	using gregorian::date;
	using gregorian::date_duration;
	date ret = boost_date_from_julian_int(value(m_data->next_date));
	if (n == 0)
	{
		return ret;
	}
	Frequency const freq = value(m_data->frequency);

	JEWEL_ASSERT
	(	numeric_cast<Size>(numeric_limits<decltype(freq.num_steps())>::max())
			<= numeric_limits<Size>::max()
	);
	// Due to preconditions when setting number of steps in Frequency, we
	// know that
	JEWEL_ASSERT (freq.num_steps() >= 0);

	// So we know that this won't throw.
	Size const units = numeric_cast<Size>(freq.num_steps());
	if (multiplication_is_unsafe(units, n))
	{
		JEWEL_THROW(UnsafeArithmeticException, "Unsafe multiplication.");
	}
	JEWEL_ASSERT (!multiplication_is_unsafe(units, n));
	Size const steps = units * n;
	switch (freq.step_type())
	{
	case IntervalType::days:
		ret += gregorian::date_duration(steps);
		break;
	case IntervalType::weeks:
		ret += gregorian::weeks(steps);
		break;
	case IntervalType::month_ends:
		JEWEL_ASSERT ( (next_date(0) + date_duration(1)).day() == 1);
		// FALL THROUGH
	case IntervalType::months:
		ret += gregorian::months(steps);
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	return ret;
}

Handle<OrdinaryJournal>
Repeater::fire_next()
{
	load();
	Handle<DraftJournal> const dj = draft_journal();
	Handle<OrdinaryJournal> const oj(database_connection());
	gregorian::date const next_date_elect = next_date(1);
	if 
	(	dj == database_connection().budget_instrument() &&
		dj->entries().empty()
	)
	{
		// Special case - if we're dealing with the budget instrument
		// and has no Entries, we do not cause the OrdinaryJournal to
		// be saved. We simply return it in an uninitialized state.
		// However, we still need to advance the next posting
		// date of the repeater.
		set_next_date(next_date_elect);
	}
	else
	{
		oj->mimic(*dj);
		gregorian::date const old_next_date = next_date(0);
		oj->set_date(old_next_date);
		DatabaseTransaction transaction(database_connection());
		try
		{
			oj->save();
			set_next_date(next_date_elect);
			save();
			transaction.commit();
		}
		catch (std::exception&)
		{
			set_next_date(old_next_date);
			transaction.cancel();
			throw;
		}
	}
	return oj;
}

Handle<DraftJournal>
Repeater::draft_journal()
{
	load();
	return Handle<DraftJournal>
	(	database_connection(),
		value(m_data->journal_id)
	);
}

void
Repeater::swap(Repeater& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

Repeater::Repeater(Repeater const& rhs):
	PersistentObject(rhs),
	m_data(new RepeaterData(*(rhs.m_data)))
{
}

void
Repeater::do_load()
{
	SQLStatement statement
	(	database_connection(),
		"select interval_units, interval_type_id, next_date, journal_id "
		"from repeaters where repeater_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Repeater temp(*this);
	temp.m_data->frequency = Frequency
	(	statement.extract<int>(0),
		static_cast<IntervalType>(statement.extract<int>(1))
	);
	temp.m_data->next_date =
		numeric_cast<DateRep>(statement.extract<long long>(2));
	temp.m_data->journal_id = statement.extract<Id>(3);
	swap(temp);
	JEWEL_ASSERT
	(	is_valid_date_for_interval_type
		(	boost_date_from_julian_int(value(m_data->next_date)),
			value(m_data->frequency).step_type()
		)
	);
	return;
}

void
Repeater::process_saving_statement(SQLStatement& statement)
{
	Frequency const freq = value(m_data->frequency);
	JEWEL_ASSERT
	(	is_valid_date_for_interval_type
		(	boost_date_from_julian_int(value(m_data->next_date)),
			freq.step_type()
		)
	);
	statement.bind(":interval_units", freq.num_steps());
	statement.bind
	(	":interval_type_id",
		static_cast<int>(freq.step_type())
	);
	statement.bind(":next_date", value(m_data->next_date));
	statement.bind(":journal_id", value(m_data->journal_id));
	statement.step_final();
	return;
}


void
Repeater::do_save_existing()
{
	SQLStatement updater
	(	database_connection(),
		"update repeaters set "
		"interval_units = :interval_units, "
		"interval_type_id = :interval_type_id, "
		"next_date = :next_date, "
		"journal_id = :journal_id "
		"where repeater_id = :repeater_id"
	);
	updater.bind(":repeater_id", id());
	process_saving_statement(updater);
	return;
}


void
Repeater::do_save_new()
{
	SQLStatement inserter
	(	database_connection(),
		"insert into repeaters(interval_units, interval_type_id, "
		"next_date, journal_id) values(:interval_units, "
		":interval_type_id, :next_date, :journal_id)"
	);
	process_saving_statement(inserter);
	return;
}


void
Repeater::do_ghostify()
{
	clear(m_data->frequency);
	clear(m_data->next_date);
	clear(m_data->journal_id);
	return;
}

string
Repeater::exclusive_table_name()
{
	return "repeaters";
}

string
Repeater::primary_key_name()
{
	return "repeater_id";
}

void
Repeater::mimic(Repeater& rhs)
{
	load();
	Repeater temp(*this);
	temp.set_frequency(rhs.frequency());
	temp.set_next_date(rhs.next_date(0));
	swap(temp);
	return;
}


// Implement free functions

vector<RepeaterFiringResult>
update_repeaters(PhatbooksDatabaseConnection& dbc, gregorian::date d)
{
	vector<RepeaterFiringResult> ret;
	// Read into a vector first - uneasy about reading and writing
	// at the same time.
	RepeaterTableIterator const rtit(dbc);
	RepeaterTableIterator const rtend;
	vector<Handle<Repeater> > vec(rtit, rtend);
	for (Handle<Repeater> const& repeater: vec)
	{
		while (repeater->next_date() <= d)
		{
			RepeaterFiringResult firing_result =
			{	repeater->draft_journal()->id(),
				repeater->next_date(),
				false
			};
			Handle<OrdinaryJournal> oj(repeater->database_connection());
			try
			{
				oj = repeater->fire_next();
			}
			catch (JournalOverflowException&)
			{
				ret.push_back(firing_result);
				break;
			}
			// In the special case where oj is dbc.budget_instrument(),
			// and is
			// devoid of entries, firing it does not cause any
			// OrdinaryJournal to be posted, but simply advances
			// the next posting date. In this case the returned
			// OrdinaryJournal will have no id.
#			ifndef NDEBUG
				Handle<DraftJournal> const dj = repeater->draft_journal();
				Handle<DraftJournal> const bi = dbc.budget_instrument();
#			endif
			if (oj->has_id())
			{
				firing_result.successful = true;
				JEWEL_ASSERT (firing_result.firing_date == oj->date());
				ret.push_back(firing_result);
				JEWEL_ASSERT (dj != bi || !dj->entries().empty());
			}
			else
			{
				JEWEL_ASSERT (dj == bi);
				JEWEL_ASSERT (dj->entries().empty());
				JEWEL_ASSERT (oj->entries().empty());
			}
		}
	}
	sort(ret.begin(), ret.end());
	return ret;
}



}  // namespace phatbooks

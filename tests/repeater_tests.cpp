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

#include "account.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "dcm_exceptions.hpp"
#include "dcm_tests_common.hpp"
#include "repeater.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/test/unit_test.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <memory>
#include <vector>

namespace gregorian = boost::gregorian;

using jewel::Decimal;
using gregorian::date;
using sqloxx::Handle;
using std::shared_ptr;
using std::vector;

namespace dcm
{
namespace test
{

BOOST_FIXTURE_TEST_CASE(test_repeater_next_date, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;

	Handle<DraftJournal> const dj(dbc);
	dj->set_transaction_type(TransactionType::generic);
	dj->set_comment("draft journal to test repeater");
	dj->set_name("Test");
	
	Handle<Entry> const entry1(dbc);
	entry1->
		set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
	entry1->set_comment("Test entry");
	entry1->set_amount(Decimal("-0.95"));
	entry1->set_whether_reconciled(false);
	entry1->set_transaction_side(TransactionSide::source);
	dj->push_entry(entry1);
	
	Handle<Entry> const entry2(dbc);
	entry2->
		set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "food")));
	entry2->set_comment("Test entry");
	entry2->set_amount(Decimal("0.95"));
	entry2->set_whether_reconciled(false);
	entry2->set_transaction_side(TransactionSide::destination);
	dj->push_entry(entry2);

	Handle<Repeater> const repeater1(dbc);
	repeater1->set_next_date(date(3012, 5, 30));
	BOOST_CHECK_THROW
	(	repeater1->set_frequency(Frequency(3, IntervalType::month_ends)),
		InvalidFrequencyException
	);
	BOOST_CHECK_THROW
	(	repeater1->set_frequency(Frequency(3, IntervalType::months)),
		InvalidFrequencyException
	);	
	repeater1->set_frequency(Frequency(3, IntervalType::days));
	dj->push_repeater(repeater1);
	dj->save();

	BOOST_CHECK_EQUAL(repeater1->next_date(), date(3012, 5, 30));
	BOOST_CHECK_EQUAL(repeater1->next_date(0), date(3012, 5, 30));
	BOOST_CHECK_EQUAL(repeater1->next_date(2), date(3012, 6, 5));
	BOOST_CHECK_EQUAL(repeater1->next_date(1), date(3012, 6, 2));

	Handle<Repeater> const repeater2(dbc);
	repeater2->set_next_date(date(3012, 12, 31));
	repeater2->set_frequency(Frequency(2, IntervalType::weeks));
	BOOST_CHECK_THROW
	(	repeater2->set_frequency(Frequency(2, IntervalType::months)),
		InvalidFrequencyException
	);
	dj->push_repeater(repeater2);
	dj->save();

	BOOST_CHECK_EQUAL(repeater2->next_date(2), date(3013, 1, 28));
	BOOST_CHECK_EQUAL(repeater2->next_date(1), date(3013, 1, 14));

	Handle<Repeater> const repeater3(dbc);
	repeater3->set_frequency(Frequency(1, IntervalType::months));
	BOOST_CHECK_THROW
	(	repeater3->set_next_date(date(3014, 9, 29)),
		InvalidRepeaterDateException
	);
	repeater3->set_next_date(date(3014, 9, 20));
	BOOST_CHECK_EQUAL(repeater3->next_date(5), date(3015, 2, 20));

	Handle<Repeater> const repeater4(dbc);
	repeater4->set_frequency(Frequency(1, IntervalType::month_ends));
	repeater4->set_next_date(date(2996, 1, 31));
	BOOST_CHECK_THROW
	(	repeater4->set_next_date(date(2996, 1, 30)),
		InvalidRepeaterDateException
	);
	BOOST_CHECK_EQUAL(repeater4->next_date(), date(2996, 1, 31));
	BOOST_CHECK_EQUAL(repeater4->next_date(1), date(2996, 2, 29));
	BOOST_CHECK_EQUAL(repeater4->next_date(6), date(2996, 7, 31));
	BOOST_CHECK_EQUAL(repeater4->next_date(8), date(2996, 9, 30));
	BOOST_CHECK_EQUAL(repeater4->next_date(13), date(2997, 2, 28));

	Handle<Repeater> const repeater5(dbc);
	repeater5->set_frequency(Frequency(1, IntervalType::days));
	repeater5->set_next_date(date(2900, 2, 27));
	BOOST_CHECK_EQUAL(repeater5->next_date(1), date(2900, 2, 28));
	BOOST_CHECK_EQUAL(repeater5->next_date(2), date(2900, 3, 1));

	Handle<Repeater> const repeater6(dbc);
	repeater6->set_next_date(date(3199, 2, 28));
	repeater6->set_frequency(Frequency(12, IntervalType::month_ends));
	BOOST_CHECK_EQUAL(repeater6->next_date(1), date(3200, 2, 29));

	// While we could put something here to test retrieving a Handle<Repeater>
	// by ID, there are "system Repeaters" floating
	// around, possibly ones that have been saved and then
	// deleted, and these make the particular Id of each Repeater non-obvious.
}

BOOST_FIXTURE_TEST_CASE(test_repeater_fire_next, TestFixture)
{
	DcmDatabaseConnection& dbc = *pdbc;

	Handle<DraftJournal> const dj1(dbc);
	dj1->set_transaction_type(TransactionType::generic);
	dj1->set_comment("journal to test repeater");
	dj1->set_name(wxString("Test"));  // wxString is optional
	
	Handle<Entry> const entry1a(dbc);
	entry1a->
		set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "cash")));
	entry1a->set_comment(wxString("Test entry")); // wxString is optional
	entry1a->set_amount(Decimal("-1090.95"));
	entry1a->set_whether_reconciled(false);
	entry1a->set_transaction_side(TransactionSide::source);
	dj1->push_entry(entry1a);
	
	Handle<Entry> const entry1b(dbc);
	entry1b->
		set_account(Handle<Account>(dbc, Account::id_for_name(dbc, "food")));
	entry1b->set_comment("Test entry");
	entry1b->set_amount(Decimal("1090.95"));
	entry1b->set_whether_reconciled(false);
	entry1b->set_transaction_side(TransactionSide::destination);
	dj1->push_entry(entry1b);

	Handle<Repeater> const repeater1(dbc);
	repeater1->set_frequency(Frequency(2, IntervalType::weeks));
	repeater1->set_next_date(date(3012, 7, 30));
	dj1->push_repeater(repeater1);

	dj1->save();

	Handle<Repeater> const repeater1b = repeater1;
	Handle<OrdinaryJournal> const oj1b = repeater1b->fire_next();
	BOOST_CHECK_EQUAL(oj1b->comment(), "journal to test repeater");
	BOOST_CHECK_EQUAL(oj1b->comment(), wxString("journal to test repeater"));
	BOOST_CHECK_EQUAL(oj1b->date(), date(3012, 7, 30));
	BOOST_CHECK_EQUAL(repeater1->next_date(), date(3012, 8, 13));
	BOOST_CHECK
	(	oj1b->transaction_type() ==
		TransactionType::generic
	);

	Handle<OrdinaryJournal> const oj1c = oj1b;
	BOOST_CHECK_EQUAL(oj1c->date(), date(3012, 7, 30));
	BOOST_CHECK_EQUAL(oj1c->comment(), "journal to test repeater");
	BOOST_CHECK_EQUAL(oj1c->entries().size(), unsigned(2));
	BOOST_CHECK
	(	oj1c->transaction_type() ==
		TransactionType::generic
	);

	repeater1b->fire_next();
	repeater1b->fire_next();

	Handle<OrdinaryJournal> const oj3(dbc, oj1c->id() + 1);
	Handle<OrdinaryJournal> const oj4(dbc, oj1c->id() + 2);

	BOOST_CHECK_EQUAL(oj3->date(), date(3012, 8, 13));
	BOOST_CHECK_EQUAL(oj4->date(), date(3012, 8, 27));
	BOOST_CHECK_EQUAL(oj3->comment(), oj4->comment());
	vector<Handle<Entry> >::const_iterator it3 = ++oj3->entries().begin();
	vector<Handle<Entry> >::const_iterator it4 = ++oj4->entries().begin();
	BOOST_CHECK_EQUAL((*it3)->amount(), (*it4)->amount());

	repeater1b->set_next_date(date(3012, 10, 5));
	repeater1b->set_frequency(Frequency(3, IntervalType::months));
	repeater1b->save();

	Handle<OrdinaryJournal> const oj5 = repeater1b->fire_next();
	BOOST_CHECK_EQUAL(oj5->date(), date(3012, 10, 5));
	Handle<OrdinaryJournal> const oj6 = repeater1b->fire_next();
	BOOST_CHECK_EQUAL(oj6->date(), date(3013, 1, 5));

	repeater1b->set_frequency(Frequency(1, IntervalType::days));
	repeater1b->set_next_date(date(3013, 4, 30));
	repeater1b->set_frequency(Frequency(2, IntervalType::month_ends));
	dj1->save();

	Handle<OrdinaryJournal> const oj7 = repeater1b->fire_next();
	BOOST_CHECK_EQUAL(oj7->date(), date(3013, 4, 30));
	Handle<OrdinaryJournal> const oj8 = repeater1b->fire_next();
	BOOST_CHECK_EQUAL(oj8->date(), date(3013, 6, 30));
	Handle<OrdinaryJournal> const oj9 = repeater1b->fire_next();
	BOOST_CHECK_EQUAL(oj9->date(), date(3013, 8, 31));
	BOOST_CHECK_EQUAL(repeater1b->next_date(), date(3013, 10, 31));

	repeater1b->set_frequency(Frequency(3, IntervalType::days));
	repeater1b->save();
	BOOST_CHECK_EQUAL(repeater1b->fire_next()->date(), date(3013, 10, 31));
	BOOST_CHECK_EQUAL(repeater1b->fire_next()->date(), date(3013, 11, 3));
}

}  // namespace test
}  // namespace dcm

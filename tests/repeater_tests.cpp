// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "account.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_tests_common.hpp"
#include "repeater.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle.hpp>
#include <UnitTest++/UnitTest++.h>
#include <wx/string.h>
#include <memory>
#include <vector>

namespace gregorian = boost::gregorian;

using jewel::Decimal;
using gregorian::date;
using sqloxx::Handle;
using std::shared_ptr;
using std::vector;

namespace phatbooks
{
namespace test
{

TEST_FIXTURE(TestFixture, test_repeater_next_date)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;

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
	repeater1->set_frequency(Frequency(3, IntervalType::days));
	repeater1->set_next_date(date(3012, 5, 30));
	dj->push_repeater(repeater1);
	dj->save();

	CHECK_EQUAL(repeater1->next_date(), date(3012, 5, 30));
	CHECK_EQUAL(repeater1->next_date(0), date(3012, 5, 30));
	CHECK_EQUAL(repeater1->next_date(2), date(3012, 6, 5));
	CHECK_EQUAL(repeater1->next_date(1), date(3012, 6, 2));

	Handle<Repeater> const repeater2(dbc);
	repeater2->set_frequency(Frequency(2, IntervalType::weeks));
	repeater2->set_next_date(date(3012, 12, 31));
	dj->push_repeater(repeater2);
	dj->save();

	CHECK_EQUAL(repeater2->next_date(2), date(3013, 1, 28));
	CHECK_EQUAL(repeater2->next_date(1), date(3013, 1, 14));

	Handle<Repeater> const repeater3(dbc);
	repeater3->set_frequency(Frequency(1, IntervalType::months));
	repeater3->set_next_date(date(3014, 9, 20));
	CHECK_EQUAL(repeater3->next_date(5), date(3015, 2, 20));

	Handle<Repeater> const repeater4(dbc);
	repeater4->set_frequency(Frequency(1, IntervalType::month_ends));
	repeater4->set_next_date(date(2996, 1, 31));
	CHECK_EQUAL(repeater4->next_date(), date(2996, 1, 31));
	CHECK_EQUAL(repeater4->next_date(1), date(2996, 2, 29));
	CHECK_EQUAL(repeater4->next_date(6), date(2996, 7, 31));
	CHECK_EQUAL(repeater4->next_date(8), date(2996, 9, 30));
	CHECK_EQUAL(repeater4->next_date(13), date(2997, 2, 28));

	Handle<Repeater> const repeater5(dbc);
	repeater5->set_frequency(Frequency(1, IntervalType::days));
	repeater5->set_next_date(date(2900, 2, 27));
	CHECK_EQUAL(repeater5->next_date(1), date(2900, 2, 28));
	CHECK_EQUAL(repeater5->next_date(2), date(2900, 3, 1));

	Handle<Repeater> const repeater6(dbc);
	repeater6->set_frequency(Frequency(12, IntervalType::month_ends));
	repeater6->set_next_date(date(3199, 2, 28));
	CHECK_EQUAL(repeater6->next_date(1), date(3200, 2, 29));

	// TODO Put something here to test retrieving a Handle<Repeater> by
	// ID. But note, there are "system Repeaters" floating
	// around, possibly ones that have been saved and then
	// deleted, and these make the ID non-obvious.
}

TEST_FIXTURE(TestFixture, test_repeater_firings_till)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;

	Handle<Repeater> const repeater1(dbc);
	repeater1->set_frequency(Frequency(5, IntervalType::days));
	repeater1->set_next_date(date(3000, 5, 3));
	shared_ptr<vector<date> > firings1 =
		repeater1->firings_till(date(2999, 5, 2));
	CHECK(firings1->empty());
	firings1 = repeater1->firings_till(date(3000, 5, 4));
	CHECK_EQUAL(firings1->size(), unsigned(1));
	CHECK_EQUAL((*firings1)[0], date(3000, 5, 3));
	firings1 = repeater1->firings_till(date(3000, 5, 2));
	CHECK(firings1->empty());
	firings1 = repeater1->firings_till(date(3000, 5, 3));
	CHECK_EQUAL(firings1->size(), unsigned(1));
	CHECK_EQUAL((*firings1)[0], date(3000, 5, 3));
	firings1 = repeater1->firings_till(date(3000, 5, 20));
	CHECK_EQUAL(firings1->size(), unsigned(4));
	CHECK_EQUAL((*firings1)[0], date(3000, 5, 3));
	CHECK_EQUAL((*firings1)[1], date(3000, 5, 8));
	CHECK_EQUAL((*firings1)[2], date(3000, 5, 13));
	CHECK_EQUAL((*firings1)[3], date(3000, 5, 18));

	Handle<Repeater> const repeater2(dbc);
	repeater2->set_frequency(Frequency(3, IntervalType::month_ends));
	repeater2->set_next_date(date(3012, 12, 31));
	shared_ptr<vector<date> > firings2;
	firings2 = repeater2->firings_till(date(3012, 12, 30));
	CHECK(firings2->empty());
	firings2 = repeater2->firings_till(date(3013, 2, 28));
	CHECK_EQUAL(firings2->size(), unsigned(1));
	CHECK_EQUAL((*firings2)[0], date(3012, 12, 31));
	firings2 = repeater2->firings_till(date(3013, 12, 31));
	CHECK_EQUAL(firings2->size(), unsigned(5));
	CHECK_EQUAL((*firings2)[4], date(3013, 12, 31));
	CHECK_EQUAL((*firings2)[3], date(3013, 9, 30));
	CHECK_EQUAL((*firings2)[2], date(3013, 6, 30));
	CHECK_EQUAL((*firings2)[1], date(3013, 3, 31));
	CHECK_EQUAL((*firings2)[0], date(3012, 12, 31));
}

TEST_FIXTURE(TestFixture, test_repeater_fire_next)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;

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
	CHECK_EQUAL(oj1b->comment(), "journal to test repeater");
	CHECK_EQUAL(oj1b->comment(), wxString("journal to test repeater"));
	CHECK_EQUAL(oj1b->date(), date(3012, 7, 30));
	CHECK_EQUAL(repeater1->next_date(), date(3012, 8, 13));
	CHECK
	(	oj1b->transaction_type() ==
		TransactionType::generic
	);

	Handle<OrdinaryJournal> const oj1c = oj1b;
	CHECK_EQUAL(oj1c->date(), date(3012, 7, 30));
	CHECK_EQUAL(oj1c->comment(), "journal to test repeater");
	CHECK_EQUAL(oj1c->entries().size(), unsigned(2));
	CHECK
	(	oj1c->transaction_type() ==
		TransactionType::generic
	);

	repeater1b->fire_next();
	repeater1b->fire_next();

	Handle<OrdinaryJournal> const oj3(dbc, oj1c->id() + 1);
	Handle<OrdinaryJournal> const oj4(dbc, oj1c->id() + 2);

	CHECK_EQUAL(oj3->date(), date(3012, 8, 13));
	CHECK_EQUAL(oj4->date(), date(3012, 8, 27));
	CHECK_EQUAL(oj3->comment(), oj4->comment());
	vector<Handle<Entry> >::const_iterator it3 = ++oj3->entries().begin();
	vector<Handle<Entry> >::const_iterator it4 = ++oj4->entries().begin();
	CHECK_EQUAL((*it3)->amount(), (*it4)->amount());

}






}  // namespace test
}  // namespace phatbooks

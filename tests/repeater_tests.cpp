#include "account.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include "repeater.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <UnitTest++/UnitTest++.h>
#include <vector>

namespace gregorian = boost::gregorian;

using boost::shared_ptr;
using jewel::Decimal;
using gregorian::date;
using std::vector;

namespace phatbooks
{
namespace test
{

TEST_FIXTURE(TestFixture, test_repeater_next_date)
{
	DraftJournal dj(dbc);
	dj.set_whether_actual(true);
	dj.set_comment("draft journal to test repeater");
	dj.set_name("Test");
	
	Entry entry1(dbc);
	entry1.set_account(Account(dbc, "cash"));
	entry1.set_comment("Test entry");
	entry1.set_amount(Decimal("-0.95"));
	entry1.set_whether_reconciled(false);
	dj.add_entry(entry1);
	
	Entry entry2(dbc);
	entry2.set_account(Account(dbc, "food"));
	entry2.set_comment("Test entry");
	entry2.set_amount(Decimal("0.95"));
	entry2.set_whether_reconciled(false);
	dj.add_entry(entry2);

	Repeater repeater1(dbc);
	repeater1.set_interval_type(interval_type::days);
	repeater1.set_interval_units(3);
	repeater1.set_next_date(date(2012, 5, 30));
	dj.add_repeater(repeater1);
	dj.save();

	CHECK_EQUAL(repeater1.next_date(), date(2012, 5, 30));
	CHECK_EQUAL(repeater1.next_date(0), date(2012, 5, 30));
	CHECK_EQUAL(repeater1.next_date(2), date(2012, 6, 5));
	CHECK_EQUAL(repeater1.next_date(1), date(2012, 6, 2));

	Repeater repeater2(dbc);
	repeater2.set_interval_type(interval_type::weeks);
	repeater2.set_interval_units(2);
	repeater2.set_next_date(date(2012, 12, 31));
	dj.add_repeater(repeater2);
	dj.save();

	CHECK_EQUAL(repeater2.next_date(2), date(2013, 1, 28));
	CHECK_EQUAL(repeater2.next_date(1), date(2013, 1, 14));

	Repeater repeater3(dbc);
	repeater3.set_interval_type(interval_type::months);
	repeater3.set_interval_units(1);
	repeater3.set_next_date(date(2014, 9, 20));
	CHECK_EQUAL(repeater3.next_date(5), date(2015, 2, 20));

	Repeater repeater4(dbc);
	repeater4.set_interval_type(interval_type::month_ends);
	repeater4.set_interval_units(1);
	repeater4.set_next_date(date(1996, 1, 31));
	CHECK_EQUAL(repeater4.next_date(), date(1996, 1, 31));
	CHECK_EQUAL(repeater4.next_date(1), date(1996, 2, 29));
	CHECK_EQUAL(repeater4.next_date(6), date(1996, 7, 31));
	CHECK_EQUAL(repeater4.next_date(8), date(1996, 9, 30));
	CHECK_EQUAL(repeater4.next_date(13), date(1997, 2, 28));

	Repeater repeater5(dbc);
	repeater5.set_interval_type(interval_type::days);
	repeater5.set_interval_units(1);
	repeater5.set_next_date(date(1900, 2, 27));
	CHECK_EQUAL(repeater5.next_date(1), date(1900, 2, 28));
	CHECK_EQUAL(repeater5.next_date(2), date(1900, 3, 1));

	Repeater repeater6(dbc);
	repeater6.set_interval_type(interval_type::month_ends);
	repeater6.set_interval_units(12);
	repeater6.set_next_date(date(1999, 2, 28));
	CHECK_EQUAL(repeater6.next_date(1), date(2000, 2, 29));

	Repeater repeater1b(dbc, 1);
	CHECK_EQUAL(repeater1b.next_date(), date(2012, 5, 30));
	CHECK_EQUAL(repeater1b.next_date(1), date(2012, 6, 2));
}

TEST_FIXTURE(TestFixture, test_repeater_firings_till)
{
	Repeater repeater1(dbc);
	repeater1.set_interval_type(interval_type::days);
	repeater1.set_interval_units(5);
	repeater1.set_next_date(date(2000, 5, 3));
	shared_ptr<vector<date> > firings1 =
		repeater1.firings_till(date(1999, 5, 2));
	CHECK(firings1->empty());
	firings1 = repeater1.firings_till(date(2000, 5, 4));
	CHECK_EQUAL(firings1->size(), 1);
	CHECK_EQUAL((*firings1)[0], date(2000, 5, 3));
	firings1 = repeater1.firings_till(date(2000, 5, 2));
	CHECK(firings1->empty());
	firings1 = repeater1.firings_till(date(2000, 5, 3));
	CHECK_EQUAL(firings1->size(), 1);
	CHECK_EQUAL((*firings1)[0], date(2000, 5, 3));
	firings1 = repeater1.firings_till(date(2000, 5, 20));
	CHECK_EQUAL(firings1->size(), 4);
	CHECK_EQUAL((*firings1)[0], date(2000, 5, 3));
	CHECK_EQUAL((*firings1)[1], date(2000, 5, 8));
	CHECK_EQUAL((*firings1)[2], date(2000, 5, 13));
	CHECK_EQUAL((*firings1)[3], date(2000, 5, 18));

	Repeater repeater2(dbc);
	repeater2.set_interval_type(interval_type::month_ends);
	repeater2.set_interval_units(3);
	repeater2.set_next_date(date(2012, 12, 31));
	shared_ptr<vector<date> > firings2;
	firings2 = repeater2.firings_till(date(2012, 12, 30));
	CHECK(firings2->empty());
	firings2 = repeater2.firings_till(date(2013, 2, 28));
	CHECK_EQUAL(firings2->size(), 1);
	CHECK_EQUAL((*firings2)[0], date(2012, 12, 31));
	firings2 = repeater2.firings_till(date(2013, 12, 31));
	CHECK_EQUAL(firings2->size(), 5);
	CHECK_EQUAL((*firings2)[4], date(2013, 12, 31));
	CHECK_EQUAL((*firings2)[3], date(2013, 9, 30));
	CHECK_EQUAL((*firings2)[2], date(2013, 6, 30));
	CHECK_EQUAL((*firings2)[1], date(2013, 3, 31));
	CHECK_EQUAL((*firings2)[0], date(2012, 12, 31));
}

TEST_FIXTURE(TestFixture, test_repeater_fire_next)
{
	DraftJournal dj1(dbc);
	dj1.set_whether_actual(true);
	dj1.set_comment("journal to test repeater");
	dj1.set_name("Test");
	
	Entry entry1a(dbc);
	entry1a.set_account(Account(dbc, "cash"));
	entry1a.set_comment("Test entry");
	entry1a.set_amount(Decimal("-1090.95"));
	entry1a.set_whether_reconciled(false);
	dj1.add_entry(entry1a);
	
	Entry entry1b(dbc);
	entry1b.set_account(Account(dbc, "food"));
	entry1b.set_comment("Test entry");
	entry1b.set_amount(Decimal("1090.95"));
	entry1b.set_whether_reconciled(false);
	dj1.add_entry(entry1b);

	Repeater repeater1(dbc);
	repeater1.set_interval_type(interval_type::weeks);
	repeater1.set_interval_units(2);
	repeater1.set_next_date(date(2012, 7, 30));
	dj1.add_repeater(repeater1);

	dj1.save();

	Repeater repeater1b(dbc, 1);
	OrdinaryJournal oj1b = repeater1b.fire_next();
	CHECK_EQUAL(oj1b.comment(), "journal to test repeater");
	CHECK_EQUAL(oj1b.date(), date(2012, 7, 30));
	CHECK_EQUAL(repeater1.next_date(), date(2012, 8, 13));

	OrdinaryJournal oj1c(dbc, 2);
	CHECK_EQUAL(oj1c.date(), date(2012, 7, 30));
	CHECK_EQUAL(oj1c.comment(), "journal to test repeater");
	CHECK_EQUAL(oj1c.entries().size(), 2);

	repeater1b.fire_next();
	repeater1b.fire_next();

	OrdinaryJournal oj3(dbc, 3);
	OrdinaryJournal oj4(dbc, 4);

	CHECK_EQUAL(oj3.date(), date(2012, 8, 13));
	CHECK_EQUAL(oj4.date(), date(2012, 8, 27));
	CHECK_EQUAL(oj4.id(), 4);
	CHECK_EQUAL(oj3.comment(), oj4.comment());
	vector<Entry>::const_iterator it3 = ++oj3.entries().begin();
	vector<Entry>::const_iterator it4 = ++oj4.entries().begin();
	CHECK_EQUAL(it3->amount(), it4->amount());

}


TEST_FIXTURE(TestFixture, test_repeater_frequency_phrase)
{
	Repeater repeater1(dbc);
	repeater1.set_interval_type(interval_type::days);
	repeater1.set_interval_units(1);
	CHECK_EQUAL(frequency_description(repeater1), "every day");

	Repeater repeater2(dbc);
	repeater2.set_interval_type(interval_type::days);
	repeater2.set_interval_units(12);
	CHECK_EQUAL(frequency_description(repeater2), "every 12 days");

	Repeater repeater3(dbc);
	repeater3.set_interval_type(interval_type::weeks);
	repeater3.set_interval_units(1);
	CHECK_EQUAL(frequency_description(repeater3), "every week");
	
	Repeater repeater4(dbc);
	repeater4.set_interval_type(interval_type::weeks);
	repeater4.set_interval_units(2);
	CHECK_EQUAL(frequency_description(repeater4), "every 2 weeks");

	Repeater repeater5(dbc);
	repeater5.set_interval_type(interval_type::months);
	repeater5.set_interval_units(3);
	CHECK_EQUAL(frequency_description(repeater5), "every 3 months");

	Repeater repeater6(dbc);
	repeater6.set_interval_type(interval_type::months);
	repeater6.set_interval_units(1);
	CHECK_EQUAL(frequency_description(repeater6), "every month");

	Repeater repeater7(dbc);
	repeater7.set_interval_type(interval_type::months);
	repeater7.set_interval_units(12);
	CHECK_EQUAL(frequency_description(repeater7), "every 12 months");

	Repeater repeater8(dbc);
	repeater8.set_interval_type(interval_type::month_ends);
	repeater8.set_interval_units(1);
	CHECK_EQUAL
	(	frequency_description(repeater8),
		"every month, on the last day of the month"
	);

	Repeater repeater9(dbc);
	repeater9.set_interval_type(interval_type::month_ends);
	repeater9.set_interval_units(10);
	CHECK_EQUAL
	(	frequency_description(repeater9),
		"every 10 months, on the last day of the month"
	);
}





}  // namespace test
}  // namespace phatbooks

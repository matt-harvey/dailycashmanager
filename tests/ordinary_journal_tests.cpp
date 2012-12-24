#include "phatbooks_tests_common.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <unittest++/UnitTest++.h>
#include <vector>

using boost::gregorian::date;
using jewel::Decimal;
using std::vector;

#include <iostream>
#include <stdexcept>
using std::cerr;
using std::endl;

namespace phatbooks
{
namespace test
{

TEST_FIXTURE(TestFixture, test_ordinary_journal_mimic)
{
	Journal journal1;
	journal1.set_whether_actual(true);
	journal1.set_comment("igloo");

	Entry entry1a(dbc);
	entry1a.set_account(Account(dbc, "cash"));
	entry1a.set_comment("igloo entry a");
	entry1a.set_whether_reconciled(true);
	entry1a.set_amount(Decimal("0.99"));
	journal1.add_entry(entry1a);

	Entry entry1b(dbc);
	entry1b.set_account(Account(dbc, "food"));
	entry1b.set_comment("igloo entry b");
	entry1b.set_whether_reconciled(false);
	entry1b.set_amount(Decimal("50.09"));
	journal1.add_entry(entry1b);
	OrdinaryJournal oj1(dbc);
	oj1.set_date(date(2000, 1, 5));
	oj1.mimic(journal1);
	CHECK_EQUAL(oj1.date(), date(2000, 1, 5));
	CHECK_EQUAL(oj1.is_actual(), true);
	CHECK_EQUAL(oj1.comment(), "igloo");
	CHECK_EQUAL(oj1.entries().size(), 2);
	oj1.save();
	CHECK(!oj1.entries().empty());
	for
	(	vector<Entry>::const_iterator it1 = oj1.entries().begin(),
		  end = oj1.entries().end();
		it1 != end;
		++it1
	)
	{
		CHECK( (it1->id() == 1) || (it1->id() == 2) );
		if (it1->id() == 1)
		{
			CHECK_EQUAL(it1->account().id(), Account(dbc, "cash").id());
			CHECK_EQUAL(it1->comment(), "igloo entry a");
			CHECK_EQUAL(it1->amount(), Decimal("0.99"));
			CHECK_EQUAL(it1->is_reconciled(), true);
		}
		else
		{
			CHECK_EQUAL(it1->id(), 2);
			CHECK_EQUAL(it1->is_reconciled(), false);
			CHECK_EQUAL(it1->amount(), Decimal("50.09"));
			CHECK_EQUAL(it1->comment(), "igloo entry b");
			CHECK_EQUAL(it1->account().id(), Account(dbc, "food").id());
		}
	}
	DraftJournal dj2(dbc);
	dj2.set_whether_actual(false);
	dj2.set_comment("steam engine");
	dj2.set_name("some journal");
	
	Entry entry2a(dbc);
	entry2a.set_account(Account(dbc, "food"));
	entry2a.set_comment("steam");
	entry2a.set_amount(Decimal("-1000.95"));
	entry2a.set_whether_reconciled(false);
	dj2.add_entry(entry2a);
	
	oj1.mimic(dj2);

	CHECK_EQUAL(oj1.is_actual(), false);
	CHECK_EQUAL(oj1.comment(), "steam engine");
	CHECK_EQUAL(oj1.entries().size(), 1);
	CHECK_EQUAL(oj1.date(), date(2000, 1, 5));
	oj1.save();
	vector<Entry>::const_iterator it2 =
		oj1.entries().begin();
	for ( ; it2 != oj1.entries().end(); ++it2)
	{
		CHECK(it2->id() == 3);
		CHECK_EQUAL(it2->account().id(), Account(dbc, "food").id());
		CHECK_EQUAL(it2->comment(), "steam");
		CHECK_EQUAL(it2->is_reconciled(), false);
	}

	OrdinaryJournal oj2(dbc);
	oj2.set_whether_actual(true);
	oj2.set_comment("random");
	oj2.set_date(date(2010, 11, 30));
	
	Entry entry2b(dbc);
	entry2b.set_account(Account(dbc, "cash"));
	entry2b.set_comment("random entry");
	entry2b.set_amount(Decimal("2055.90"));
	entry2b.set_whether_reconciled(false);
	oj2.add_entry(entry2b);
	Entry entry2c(entry2b);
	entry2c.mimic(entry2b);
	entry2c.set_amount(Decimal("-2055.90"));
	oj2.add_entry(entry2c); 
	
	oj2.save();
	
	oj2.mimic(oj1);
	oj2.save();

	OrdinaryJournal oj2b(dbc, 2);
	CHECK_EQUAL(oj2b.id(), 2);
	CHECK_EQUAL(oj2b.comment(), "steam engine");
	CHECK_EQUAL(oj2b.entries().size(), 1);
	CHECK_EQUAL(oj2b.date(), date(2000, 1, 5));
	vector<Entry>::const_iterator it2b = oj2b.entries().begin();
	CHECK_EQUAL(it2b->amount(), Decimal("-1000.95"));
}


TEST_FIXTURE(TestFixture, test_ordinary_journal_is_balanced)
{
	OrdinaryJournal journal1(dbc);
	journal1.set_whether_actual(true);
	journal1.set_comment("igloo");

	Entry entry1a(dbc);
	entry1a.set_account(Account(dbc, "cash"));
	entry1a.set_comment("igloo entry a");
	entry1a.set_whether_reconciled(true);
	entry1a.set_amount(Decimal("-10.99"));
	journal1.add_entry(entry1a);

	Entry entry1b(dbc);
	entry1b.set_account(Account(dbc, "food"));
	entry1b.set_comment("igloo entry b");
	entry1b.set_whether_reconciled(false);
	entry1b.set_amount(Decimal("50.09"));
	journal1.add_entry(entry1b);
	journal1.set_date(date(2000, 1, 5));

	CHECK(!journal1.is_balanced());
	entry1b.set_amount(Decimal("10.99"));
	CHECK(journal1.is_balanced());

	journal1.save();

	OrdinaryJournal journal1b(dbc, 1);
	CHECK(journal1b.is_balanced());
	Entry entry1c(dbc);
	entry1c.set_account(Account(dbc, "food"));
	entry1c.set_comment("Ummm");
	entry1c.set_whether_reconciled(true);
	entry1c.set_amount(Decimal(0, 0));
	journal1b.add_entry(entry1c);
	CHECK(journal1b.is_balanced());
	CHECK(journal1.is_balanced());
	journal1b.save();
	
	entry1c.set_amount(Decimal("0.0000001"));
	CHECK_EQUAL(journal1.is_balanced(), false);
	CHECK(!journal1b.is_balanced());
}



}  // namespace test
}  // namespace phatbooks

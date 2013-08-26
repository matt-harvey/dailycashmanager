// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "phatbooks_tests_common.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "phatbooks_exceptions.hpp"
#include "transaction_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <UnitTest++/UnitTest++.h>
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
	PhatbooksDatabaseConnection& dbc = *pdbc;

	ProtoJournal journal1;
	journal1.set_transaction_type(transaction_type::generic_transaction);
	journal1.set_comment("igloo");

	Entry entry1a(dbc);
	entry1a.set_account(Account(dbc, "cash"));
	entry1a.set_comment("igloo entry a");
	entry1a.set_whether_reconciled(true);
	entry1a.set_amount(Decimal("0.99"));
	journal1.push_entry(entry1a);

	Entry entry1b(dbc);
	entry1b.set_account(Account(dbc, "food"));
	entry1b.set_comment("igloo entry b");
	entry1b.set_whether_reconciled(false);
	entry1b.set_amount(Decimal("-0.99"));
	journal1.push_entry(entry1b);
	OrdinaryJournal oj1(dbc);
	oj1.set_date(date(3000, 1, 5));
	oj1.mimic(journal1);
	CHECK_EQUAL(oj1.date(), date(3000, 1, 5));
	CHECK_EQUAL(oj1.is_actual(), true);
	CHECK_EQUAL(oj1.comment(), "igloo");
	CHECK_EQUAL(oj1.entries().size(), size_t(2));
	CHECK_EQUAL
	(	oj1.transaction_type(),
		transaction_type::generic_transaction
	);
	oj1.save();
	CHECK(!oj1.entries().empty());

	for
	(	vector<Entry>::const_iterator it1 = oj1.entries().begin(),
		  end = oj1.entries().end();
		it1 != end;
		++it1
	)
	{
		CHECK(it1->id() == 1 || it1->id() == 2);
		if (it1->account() == Account(dbc, "cash"))
		{
			CHECK_EQUAL(it1->comment(), "igloo entry a");
			CHECK_EQUAL(it1->amount(), Decimal("0.99"));
			CHECK_EQUAL(it1->is_reconciled(), true);
		}
		else
		{
			CHECK(it1->account() == Account(dbc, "food"));
			CHECK_EQUAL(it1->is_reconciled(), false);
			CHECK_EQUAL(it1->amount(), Decimal("-0.99"));
			CHECK_EQUAL(it1->comment(), "igloo entry b");
		}
	}
	DraftJournal dj2(dbc);
	dj2.set_transaction_type(transaction_type::envelope_transaction);
	dj2.set_comment("steam engine");
	dj2.set_name("some journal");
	
	Entry entry2a(dbc);
	entry2a.set_account(Account(dbc, "food"));
	entry2a.set_comment("steam");
	entry2a.set_amount(Decimal("0"));
	entry2a.set_whether_reconciled(false);
	dj2.push_entry(entry2a);
	
	oj1.mimic(dj2);

	CHECK_EQUAL(oj1.is_actual(), false);
	CHECK_EQUAL(oj1.comment(), "steam engine");
	CHECK_EQUAL(oj1.entries().size(), size_t(1));
	CHECK_EQUAL(oj1.date(), date(3000, 1, 5));
	CHECK_EQUAL
	(	oj1.transaction_type(),
		transaction_type::envelope_transaction
	);
	oj1.save();
	vector<Entry>::const_iterator it2 =
		oj1.entries().begin();
	for ( ; it2 != oj1.entries().end(); ++it2)
	{
		CHECK_EQUAL(it2->account().id(), Account(dbc, "food").id());
		CHECK_EQUAL(it2->comment(), "steam");
		CHECK_EQUAL(it2->is_reconciled(), false);
	}

	/* No longer providing mimic of OrdinaryJournal, so test commented out.
	OrdinaryJournal oj2(dbc);
	oj2.set_transaction_type(transaction_type::generic_transaction);
	oj2.set_comment("random");
	oj2.set_date(date(2010, 11, 30));
	
	Entry entry2b(dbc);
	entry2b.set_account(Account(dbc, "cash"));
	entry2b.set_comment("random entry");
	entry2b.set_amount(Decimal("2055.90"));
	entry2b.set_whether_reconciled(false);
	oj2.push_entry(entry2b);
	Entry entry2c(dbc);
	entry2c.mimic(entry2b);
	entry2c.set_amount(Decimal("-2055.90"));
	oj2.push_entry(entry2c); 
	
	oj2.save();
	
	oj2.mimic(oj1);
	oj2.save();

	OrdinaryJournal oj2b(dbc, 2);
	CHECK_EQUAL(oj2b.id(), 2);
	CHECK_EQUAL(oj2b.comment(), "steam engine");
	CHECK_EQUAL(oj2b.entries().size(), size_t(1));
	CHECK_EQUAL(oj2b.date(), date(2000, 1, 5));
	vector<Entry>::const_iterator it2b = oj2b.entries().begin();
	CHECK_EQUAL(it2b->amount(), Decimal("0"));
	*/
}


TEST_FIXTURE(TestFixture, test_ordinary_journal_is_balanced)
{
	PhatbooksDatabaseConnection& dbc = *pdbc;

	OrdinaryJournal journal1(dbc);
	journal1.set_transaction_type(transaction_type::generic_transaction);
	journal1.set_comment("igloo");

	Entry entry1a(dbc);
	entry1a.set_account(Account(dbc, "cash"));
	entry1a.set_comment("igloo entry a");
	entry1a.set_whether_reconciled(true);
	entry1a.set_amount(Decimal("-10.99"));
	journal1.push_entry(entry1a);
	CHECK_THROW(journal1.save(), UnbalancedJournalException);

	Entry entry1b(dbc);
	entry1b.set_account(Account(dbc, "food"));
	entry1b.set_comment("igloo entry b");
	entry1b.set_whether_reconciled(false);
	entry1b.set_amount(Decimal("50.09"));
	journal1.push_entry(entry1b);
	journal1.set_date(date(3000, 1, 5));

	CHECK(!journal1.is_balanced());
	CHECK_THROW(journal1.save(), UnbalancedJournalException);
	entry1b.set_amount(Decimal("10.99"));
	CHECK(journal1.is_balanced());

	journal1.save();

	// We already have a system journal (the budget instrument) so
	// we expect journal1b to have an id of 2, not 1.
	OrdinaryJournal journal1b(dbc, 2);

	CHECK(journal1b.is_balanced());
	Entry entry1c(dbc);
	entry1c.set_account(Account(dbc, "food"));
	entry1c.set_comment("Ummm");
	entry1c.set_whether_reconciled(true);
	entry1c.set_amount(Decimal(0, 0));
	journal1b.push_entry(entry1c);
	CHECK(journal1b.is_balanced());
	CHECK(journal1.is_balanced());
	journal1b.save();
	
	entry1c.set_amount(Decimal("0.0000001"));
	CHECK_EQUAL(journal1.is_balanced(), false);
	CHECK(!journal1b.is_balanced());
}



}  // namespace test
}  // namespace phatbooks

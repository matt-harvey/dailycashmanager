/** \file test.cpp
 *
 * \brief Executes tests.
  *
 * \author Matthew Harvey
 * \date 29 Sep 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account.hpp"
#include "entry.hpp"
#include "journal.hpp"
#include "ordinary_journal.hpp"
#include "proto_journal.hpp"
#include "tests/phatbooks_tests_common.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <UnitTest++/UnitTest++.h>
#include <stdexcept>
#include <iostream>
#include <vector>

using phatbooks::test::TestFixture;
using std::cout;
using std::endl;
using std::vector;

// TODO The tests don't run on Windows. Fix this.

int main()
{
	using namespace phatbooks;


	#if 0
	// Standalone block
	{
		// WARNING. This is all temp play. Need to revert.

		using namespace phatbooks;
		using namespace phatbooks::test;
		TestFixture fixture;

		// No failures from valgrind so far...
		
		using boost::gregorian::date;
		using jewel::Decimal;

		PhatbooksDatabaseConnection& db(fixture.dbc);

		Commodity commodity(db, 1);
		Commodity commodity_b(db, 1);

	

		// No failures from valgrind so far...
		
		OrdinaryJournal oj1(db);
		oj1.set_date(date(2000, 1, 5));
		oj1.set_comment("igloo");
		oj1.set_whether_actual(true);

		// Valgrind finds errors if we include the
		// part between ***A*** and ***B***, and then only if we
		// have object caching turned on.
		/***A***/	
		Entry entry1a(db);
		entry1a.set_account(Account(db, "cash"));
		entry1a.set_comment("igloo entry a");
		entry1a.set_whether_reconciled(true);
		entry1a.set_amount(Decimal("0.99"));
		oj1.add_entry(entry1a);

		Entry entry1b(db);
		entry1b.set_account(Account(db, "food"));
		entry1b.set_comment("igloo entry b");
		entry1b.set_whether_reconciled(false);
		entry1b.set_amount(Decimal("-0.99"));
		oj1.add_entry(entry1b);
		/***B***/

		oj1.save();

		
		

		// cout << fixture.db_filepath << endl;

		/*
		ProtoJournal journal1;
		journal1.set_whether_actual(true);
		journal1.set_comment("igloo");

		Entry entry1a(db);
		entry1a.set_account(Account(db, "cash"));
		entry1a.set_comment("igloo entry a");
		entry1a.set_whether_reconciled(true);
		entry1a.set_amount(Decimal("0.99"));
		journal1.add_entry(entry1a);

		Entry entry1b(db);
		entry1b.set_account(Account(db, "food"));
		entry1b.set_comment("igloo entry b");
		entry1b.set_whether_reconciled(false);
		entry1b.set_amount(Decimal("-0.99"));
		journal1.add_entry(entry1b);
		OrdinaryJournal oj1(db);
		oj1.set_date(date(2000, 1, 5));
		oj1.mimic(journal1);
		oj1.save();
		*/



	}

	return 0;


	#endif


	try
	{
		return UnitTest::RunAllTests();
	}
	// This seems pointless but is necessary to guarantee the stack is
	// fully unwound if an exception is thrown.
	catch (...)
	{
		throw;
	}

}

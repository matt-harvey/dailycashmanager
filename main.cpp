#include "session.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "create_database.hpp"
#include "entity.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"

// #include "UnitTest++/src/UnitTest++.h"

#include <jewel/arithmetic_exceptions.hpp>
#include <jewel/decimal.hpp>
#include <sqlite3.h>
#include <cassert>
#include <iostream>

using jewel::UnsafeArithmeticException;
using jewel::Decimal;
using phatbooks::create_database;
using std::cout;
using std::endl;

int main()
{
	sqlite3_initialize();

	create_database("/home/matthew/Workbench/versioned/phatbooks/"
	                "test_create_database.db");	

	sqlite3_shutdown();


	/*
	Session session; 
	session.run_opening_tests();
	session.execute();
	session.run_closing_tests();
	return 0;
	*/
	// return UnitTest::RunAllTests();
}

#include "session.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "create_database.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"

// #include "UnitTest++/src/UnitTest++.h"

#include <jewel/arithmetic_exceptions.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <iostream>

using jewel::UnsafeArithmeticException;
using jewel::Decimal;
using phatbooks::create_database;
using phatbooks::Session;
using std::cout;
using std::endl;

int main()
{
	Session s;

	create_database("/home/matthew/Workbench/versioned/phatbooks/"
	                "test_create_database.db");	

	return 0;
}

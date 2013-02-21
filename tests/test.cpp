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

int main()
{
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

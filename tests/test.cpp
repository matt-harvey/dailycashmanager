/** \file test.cpp
 *
 * \brief Executes tests.
  *
 * \author Matthew Harvey
 * \date 29 Sep 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "tests/phatbooks_tests_common.hpp"
#include <UnitTest++/UnitTest++.h>
#include <stdexcept>
#include <iostream>

using phatbooks::test::TestFixture;

// TODO The tests don't run on Windows. Fix this.

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

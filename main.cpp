#include "application.hpp"

// #include "UnitTest++/src/UnitTest++.h"

#include <jewel/arithmetic_exceptions.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <iostream>

using jewel::UnsafeArithmeticException;
using jewel::Decimal;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	if (argc == 0)
	{	
		cout << Decimal("0") << endl;
		return 0;
	}
	assert (argc > 0);
	Decimal sum("0");
	for (int i = 1; i != argc; ++i)
	{
		try
		{
			sum += Decimal(argv[i]);
		}
		catch (UnsafeArithmeticException&)
		{
			cout << "Argument \"" << argv[i] <<"\" is either too long, "
			        "or contains inappropriate characters, to be"
					" treated as a decimal number."
				 << endl;
			return 0;
		}
	}
	cout << sum << endl;



	/*
	Application app;
	app.run_opening_tests();
	app.execute();
	app.run_closing_tests();
	*/
	return 0;
	// return UnitTest::RunAllTests();
}

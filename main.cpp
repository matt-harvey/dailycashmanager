#include "application.hpp"
// #include "UnitTest++/src/UnitTest++.h"

int main()
{
	Application app;

	app.run_opening_tests();

	app.execute();
	
	app.run_closing_tests();

	return 0;
	// return UnitTest::RunAllTests();
}

#include "atomicity_test.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sql_statement.hpp"
#include <boost/filesystem.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::ofstream;
using std::string;


namespace sqloxx
{
namespace tests
{


int
do_atomicity_test(string const& db_filename)
{
	// To test SQL transaction handling, we do something here
	// here that will set up a transaction and then crash the program
	// part-way through the transaction - but only the "first" time it is
	// run. The "second" time, the presence of the crashed database is
	// detected, and execution instead proceeds to checking the contents
	// of the database and verifying that it handled the crash by enrolling
	// the in-progress transaction as expected.
	
	int test_result = 0;
	DatabaseConnection dbc;
	if
	(	!boost::filesystem::exists(boost::filesystem::status(db_filename))
	)
	{
		// Then we have to set up the database, and set up the conditions
		// for the test.
		dbc.open(db_filename);  // create the database file
		setup_atomicity_test(dbc);
	}
	else
	{
		// Then we know we have crashed already, and now have to inspect the
		// database file to check that it reacted as expected..
		dbc.open(db_filename);
		test_result = inspect_database_for_atomicity(dbc);
	}
	return test_result;

	// WARNING Figure out why the journal file does not delete itself
	// here - currently test.tcl is deleting this manually.
}
	

void
setup_atomicity_test(DatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table dummy"
		"("
			"col_A integer primary key autoincrement, "
			"col_B text not null, "
			"col_C text"
		");"
	);
	dbc.execute_sql
	(	"insert into dummy(col_B, col_C) values('Hello!!!', 'X');"
	);
	
	// Test failing transaction
	dbc.begin_transaction();
	dbc.execute_sql
	(	"insert into dummy(col_B, col_C) values('Bye!', 'Y');"
	);

	// Crash!
	abort();
	
	// Execution never reaches here - transaction does not complete
	assert (false);
	dbc.end_transaction();
	return;
}


int
inspect_database_for_atomicity(DatabaseConnection& dbc)
{
	int ret = 0;
	SQLStatement statement
	(	dbc,
		"select * from dummy"
	);
	bool const first_step = statement.step();  // ...into sole result row

	// We expect to step into one row of results
	if (!first_step)
	{
		// There were no results at all
		++ret;
		cout << "Atomicity test failed. 1 insertion was still expected to"
			 << " succeed; however none succeeded. "
			 << endl;
	}
	bool const second_step = statement.step();
	if (second_step)
	{
		// Then we have multiple result rows and the second insertion
		// didn't reverse as expected
		++ret;
		cout << "Atomicity test failed. SQL transaction "
			 << "did not undo as expected."
			 << endl;
	}
	if (ret == 0)
	{
		// We have exactly one result row, which means the second insert
		// was reversed as expected.
		assert (first_step);
		assert (!second_step);
		cout << "Atomicity test succeeded." << endl;
	}
	return ret;
}


}  // tests
}  // sqloxx

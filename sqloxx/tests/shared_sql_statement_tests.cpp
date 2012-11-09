#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/tests/sqloxx_tests_common.hpp"

#include <unittest++/UnitTest++.h>
#include <boost/cstdint.hpp>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

namespace sqloxx
{
namespace tests
{




TEST_FIXTURE(DatabaseConnectionFixture, test_shared_sql_statement_constructor)
{
	// Check behaviour with SQL syntax error
	CHECK_THROW
	(	SharedSQLStatement unsyntactic(dbc, "unsyntactical gobbledigook"),
		SQLiteException
	);
	
	// Check behaviour with invalid database connection
	DatabaseConnection temp_dbc;
	CHECK_THROW
	(	SharedSQLStatement unconnected_to_file
		(	temp_dbc,
			"create table dummy(Col_A text);"
		),
		InvalidConnection
	);

	// Check that InvalidConnection dominates SQLiteException (at least for
	// syntax error...)
	DatabaseConnection dbc2;
	CHECK_THROW
	(	SharedSQLStatement unsyntactic2(dbc2, "unsyntacticalsjkdf ghh"),
		InvalidConnection
	);
	

	// Now let's create an unproblematic SQLStatement. This shouldn't throw.
	SharedSQLStatement normal(dbc, "create table dummy(Col_A text)");
	
	// Doesn't compile - no default constructor.
	// SharedSQLStatement s00;
}


TEST_FIXTURE(DatabaseConnectionFixture, test_multi_statements_rejected)
{
	// These should be OK (note trailing whitespace and semicolons)
	dbc.execute_sql
	(	"create table planets(name text primary key not null, size text); "
		"create table satellites(name text unique, "
		"planet_name text references planets);"
	);
	SharedSQLStatement s0
	(	dbc,
		"insert into planets(name, size) values('Mars', 'medium'); ;;    "
	);
	SharedSQLStatement s0a
	(	dbc,
		"insert into planets(name, size) values('Saturn', 'large');"
	);
	SharedSQLStatement s0b
	(	dbc,
		"insert into planets(name, size) values('Mercury', 'small')    ;  "
	);
	s0.step_final();
	// But these should throw
	CHECK_THROW
	(
		SharedSQLStatement s1
		(	dbc,
			"insert into planets(name, size) values('Earth', 'medium'); "
			"insert into planets(name, size) values('Jupiter', 'large')"
		),
		TooManyStatements
	);
	CHECK_THROW
	(
		SharedSQLStatement s2
		(	dbc,
			"insert into planets(name, size) values('Earth', 'medium'); "
			"gooblalsdfkj(("
		),
		TooManyStatements
	);
	CHECK_THROW
	(
		SharedSQLStatement s2
		(	dbc,
			"insert into planets(name, size) values('Earth', 'medium'))); "
			"Sasdf(("
		),
		SQLiteException
	);
	// But this should be OK and with database still in valid state even after
	// the above
	CHECK(dbc.is_valid());
	SharedSQLStatement s3
	(	dbc,
		"insert into planets(name, size) values('Earth', 'medium');"
	);
	s3.step_final();
}


TEST_FIXTURE(DatabaseConnectionFixture, test_bind_and_extract_normal)
{
	dbc.execute_sql
	(	"create table dummy(Col_A integer primary key autoincrement, "
		"Col_B text not null, Col_C integer not null, Col_D integer, "
		"Col_E float)"
	);

	SharedSQLStatement statement_01
	(	dbc,
		"insert into dummy(Col_B, Col_C, Col_D, Col_E) values(:B, :C, :D, :E)"
	);
	string const hello_01("hello");
	int const x_01(30);
	boost::int64_t y_01(999999983);
	double const z_01(-20987.9873);
	statement_01.bind(":B", hello_01);
	statement_01.bind(":C", x_01);
	statement_01.bind(":D", y_01);
	statement_01.bind(":E", z_01);
	statement_01.step_final();

	string const goodbye_02("goodbye");
	int const x_02(293874);
	SharedSQLStatement statement_02
	(	dbc,
		"insert into dummy(Col_B, Col_C) values(:B, :C)"
	);
	statement_02.bind(":B", goodbye_02);
	statement_02.bind(":C", x_02);
	statement_02.step_final();
	
	SharedSQLStatement selector_01
	(	dbc,
		"select Col_B, Col_C, Col_D, Col_E from dummy where Col_A = 1"
	);
	selector_01.step();
	CHECK_EQUAL(selector_01.extract<boost::int64_t>(2), y_01);
	CHECK_EQUAL(selector_01.extract<string>(0), hello_01);
	CHECK_EQUAL(selector_01.extract<int>(1), x_01);
	CHECK_EQUAL(selector_01.extract<double>(3), z_01);

	SharedSQLStatement selector_02
	(	dbc,
		"select Col_B, Col_C, Col_D, Col_E from dummy where Col_A = 2"
	);
	selector_02.step();
	CHECK_EQUAL(selector_02.extract<string>(0), goodbye_02);
	CHECK_EQUAL(selector_02.extract<int>(1), x_02);
	selector_02.step_final();
}

TEST_FIXTURE(DatabaseConnectionFixture, test_bind_exception)
{
	dbc.execute_sql("create table dummy(Col_A integer, Col_B text);");
	SharedSQLStatement insertion_statement
	(	dbc,
		"insert into dummy(Col_A, Col_B) values(:A, :B)"
	);
	CHECK_THROW
	(	insertion_statement.bind(":a", 10),
		SQLiteException
	);
	dbc.execute_sql("insert into dummy(Col_A, Col_B) values(3, 'three')");
	dbc.execute_sql("insert into dummy(Col_A, Col_B) values(4, 'four')");
	dbc.execute_sql("insert into dummy(Col_A, Col_B) values(4, 'fourB')");
	SharedSQLStatement selector
	(	dbc,
		"select Col_A, Col_B from dummy where Col_A = :A"
	);
	selector.bind(":A", 3);
	selector.step();
	CHECK_EQUAL(selector.extract<string>(1), "three");
	selector.reset();
	selector.clear_bindings();
	selector.bind(":A", 3);
	CHECK_THROW(selector.bind(":B", 3), SQLiteException);  // Wrong type
	// Statement is now reset and bindings cleared
	bool const check = selector.step();
	CHECK_EQUAL(check, false);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_extract_value_type_exception)
{
	dbc.execute_sql("create table dummy(Col_A integer, Col_B text)");
	dbc.execute_sql("insert into dummy(Col_A, Col_B) values(3, 'hey');");
	SharedSQLStatement selection_statement
	(	dbc,
		"select Col_A, Col_B from dummy where Col_A = 3"
	);
	selection_statement.step();
	string s;
	CHECK_THROW
	(	s = selection_statement.extract<string>(0),
		ValueTypeException
	);

	dbc.execute_sql("create table dummy2(Col_X double)");
	dbc.execute_sql("insert into dummy2(Col_X) values(79610.9601)");
	SharedSQLStatement selection_statement2
	(	dbc,
		"select * from dummy2"
	);
	int x;
	CHECK_THROW
	(	x = selection_statement.extract<double>(0),
		ValueTypeException
    );
}

TEST_FIXTURE(DatabaseConnectionFixture, test_extract_index_exception_high)
{
	dbc.execute_sql("create table dummy(Col_A integer, Col_B integer)");
	dbc.execute_sql("insert into dummy(Col_A, Col_B) values(3, 10);");
	SharedSQLStatement selection_statement
	(	dbc,
		"select Col_A, Col_B from dummy where Col_A = 3"
	);
	selection_statement.step();
	int x;
	CHECK_THROW
	(	x = selection_statement.extract<int>(2),
		ResultIndexOutOfRange
	);
	// Statement unchanged and still good after exception thrown
	CHECK_EQUAL(selection_statement.extract<int>(1), 10);
	CHECK_EQUAL(selection_statement.extract<int>(0), 3);
	CHECK_EQUAL(selection_statement.step(), false);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_extract_index_exception_low)
{
	dbc.execute_sql("create table dummy(Col_A text, Col_B integer)");
	dbc.execute_sql("insert into dummy(Col_A, Col_B) values('Hello', 9)");
	SharedSQLStatement selection_statement
	(	dbc,
		"select Col_A, Col_B from dummy"
	);
	selection_statement.step();
	string s;
	CHECK_THROW
	(	s = selection_statement.extract<string>(-1),
		ResultIndexOutOfRange
	);
	// Statement unchanged and still good after exception thrown
	CHECK_EQUAL(selection_statement.extract<string>(0), "Hello");
	CHECK_EQUAL(selection_statement.extract<int>(1), 9);
	CHECK_EQUAL(selection_statement.step(), false);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_step)
{
	// Create table
	dbc.execute_sql
	(	"create table planets(name text not null unique, size text)"
	);

	// Populate table
	SharedSQLStatement insertion_statement_01
	(	dbc,
		"insert into planets(name, size) values('Mercury', 'small')"
	);
	bool const insertion_step_01 = insertion_statement_01.step();
	CHECK_EQUAL(insertion_step_01, false);
	SharedSQLStatement insertion_statement_02
	(	dbc,
		"insert into planets(name, size) values('Venus', 'medium')"
	);
	bool const insertion_step_02 = insertion_statement_02.step();
	CHECK_EQUAL(insertion_step_02, false);
	SharedSQLStatement insertion_statement_03
	(	dbc,
		"insert into planets(name, size) values('Earth', 'medium')"
	);
	bool const insertion_step_03 = insertion_statement_03.step();
	CHECK(!insertion_step_03);
	
	// Extract from table
	SharedSQLStatement selection_statement_01
	(	dbc,
		"select name, size from planets where size = 'medium'"
	);
	bool const first_step = selection_statement_01.step();
	CHECK(first_step);
	bool const second_step = selection_statement_01.step();
	CHECK_EQUAL(second_step, true);
	bool const third_step = selection_statement_01.step();
	CHECK(!third_step);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_step_final)
{
	// Create table
	dbc.execute_sql
	(	"create table planets(name text not null unique, size text)"
	);

	// Populate table
	SharedSQLStatement insertion_statement_01
	(	dbc,
		"insert into planets(name, size) values('Jupiter', 'large')"
	);
	insertion_statement_01.step_final();  // Shouldn't throw
	SharedSQLStatement insertion_statement_02
	(	dbc,
		"insert into planets(name, size) values('Saturn', 'large')"
	);
	insertion_statement_02.step_final();

	// Extract from table
	SharedSQLStatement selection_statement_01
	(	dbc,
		"select name, size from planets where size = 'large' order by name"
	);
	selection_statement_01.step();
	CHECK_THROW(selection_statement_01.step_final(), UnexpectedResultRow);
	selection_statement_01.step();
	CHECK_EQUAL(selection_statement_01.extract<string>(0), "Jupiter");
}

TEST_FIXTURE(DatabaseConnectionFixture, test_reset)
{
	// Create table
	dbc.execute_sql
	(	"create table planets(name text not null unique, visited integer)"
	);

	// Populate table
	dbc.execute_sql("insert into planets(name, visited) values('Earth', 1)");
	dbc.execute_sql
	(	"insert into planets(name, visited) values('Neptune', 0)"
	);
	dbc.execute_sql("insert into planets(name, visited) values('Uranus', 0)");

	// Extract from table
	SharedSQLStatement selection_statement
	(	dbc,
		"select name from planets where visited = :visited order by name"
	);
	selection_statement.bind(":visited", 1);
	selection_statement.step();
	CHECK_EQUAL(selection_statement.extract<string>(0), "Earth");
	selection_statement.reset();
	selection_statement.bind(":visited", 0);
	selection_statement.step();
	CHECK_EQUAL(selection_statement.extract<string>(0), "Neptune");
	selection_statement.step();
	CHECK_EQUAL(selection_statement.extract<string>(0), "Uranus");
	bool const final_step = selection_statement.step();
	CHECK_EQUAL(final_step, false);
	selection_statement.reset();
	selection_statement.step();
	CHECK_EQUAL(selection_statement.extract<string>(0), "Neptune");
	selection_statement.step();
	CHECK_EQUAL(selection_statement.extract<string>(0), "Uranus");
	bool const final_step_again = selection_statement.step();
	CHECK(!final_step_again);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_clear_bindings_01)
{
	// Create table
	dbc.execute_sql
	(	"create table planets(name text not null, visited integer)"
	);
	// Populate the table
	SharedSQLStatement insertion_statement_01
	(	dbc,
		"insert into planets(name, visited) values(:planet, :visited)"
	);
	insertion_statement_01.bind(":planet", "Earth");
	insertion_statement_01.bind(":visited", 1);
	bool const step_01 = insertion_statement_01.step();
	CHECK(!step_01);
	insertion_statement_01.reset();
	bool const step_02 = insertion_statement_01.step();  // Should be fine
	CHECK(!step_02);
	insertion_statement_01.reset();
	insertion_statement_01.clear_bindings();
	CHECK_THROW(insertion_statement_01.step(), SQLiteConstraint);
}
	

TEST_FIXTURE(DatabaseConnectionFixture, test_clear_bindings_02)
{
	// Create table
	dbc.execute_sql
	(	"create table planets(name text unique, size text)"
	);
	// Populate the table
	SharedSQLStatement insertion_statement_01
	(	dbc,
		"insert into planets(name, size) values(:planet, :size)"
	);
	insertion_statement_01.bind(":planet", "Earth");
	insertion_statement_01.bind(":size", "medium");
	bool const step_01 = insertion_statement_01.step();
	CHECK(!step_01);
	insertion_statement_01.reset();
	bool const step_02 = insertion_statement_01.step();  // Should be fine
	CHECK(!step_02);
	insertion_statement_01.reset();
	insertion_statement_01.clear_bindings();
	// Should be OK to insert with nulls, as no constraints here
	bool const step_03 = insertion_statement_01.step();
	CHECK(!step_03);
	
	// Inspect the table
	SharedSQLStatement selection_statement_01(dbc, "select * from planets");
	selection_statement_01.step();  // Earth
	selection_statement_01.step();  // Earth again
	selection_statement_01.step();  // Nulls
	selection_statement_01.step_final();
}
	

TEST_FIXTURE(DatabaseConnectionFixture, test_locking_mechanism)
{
	// Test that locking prevents two SharedSQLStatements with the
	// same text from sharing the same underlying SQLStatement when
	// they are in the same scope.
	dbc.execute_sql
	(	"create table planets(name text unique, size text)"
	);
	// Populate the table
	dbc.execute_sql
	(	"insert into planets(name, size) values('Earth', 'Medium')"
	);
	// Now the SharedSQLStatements
	string const text("select size from planets where name = 'Earth'");
	SharedSQLStatement s0(dbc, text);
	SharedSQLStatement s1(dbc, text);
	CHECK_EQUAL(s0.step(), true);
	CHECK_EQUAL(s0.step(), false);
	CHECK_EQUAL(s1.step(), true);
	CHECK_EQUAL(s1.step(), false);
}


TEST_FIXTURE(DatabaseConnectionFixture, ReuseSQLStatementAfterError1)
{
	// Setting up
	dbc.execute_sql
	(	"create table planets(name text primary key not null, size text); "
		"create table satellites(name text unique, "
		"planet_name text references planets);"
	);
	SharedSQLStatement s0
	(	dbc,
		"select name from planets where name = 'Mars';"
	);
	s0.step_final();
	SharedSQLStatement s1
	(	dbc,
		"insert into planets(name, size) values('Mars', 'small');"
	);
	s1.step_final();
	SharedSQLStatement s2
	(	dbc,
		"insert into planets(name, size) values('Earth', 'medium');"
	);
	s2.step_final();
	SharedSQLStatement s3
	(	dbc,
		"insert into planets(name, size) values('Venus', 'medium');"
	);
	s3.step_final();

	// Here's the statement we will stuff up the state of
	string const selector_text =
		"select name from planets where size = 'small';";
	for (int i = 0; i != 1; ++i)
	{
		SharedSQLStatement selector0(dbc, selector_text);
		selector0.step();
		// Extract the wrong type
		try
		{
			int x = selector0.extract<int>(0);
			++x;  // Silence compiler re. unused variable;
		}
		catch (...)
		{
		}
	}
	// But this is still OK
	for (int i = 0; i != 1; ++i)
	{
		SharedSQLStatement selector1(dbc, selector_text);
		selector1.step();
		string x = selector1.extract<string>(0);
		CHECK_EQUAL(x, "Mars");
	}
}


TEST_FIXTURE(DatabaseConnectionFixture, ReuseSQLStatementAfterError2)
{
	// Setting up
	dbc.execute_sql
	(	"create table planets(name text primary key not null, size text); "
		"create table satellites(name text unique, "
		"planet_name text references planets);"
	);
	SharedSQLStatement s0
	(	dbc,
		"select name from planets where name = 'Mars';"
	);
	s0.step_final();
	SharedSQLStatement s1
	(	dbc,
		"insert into planets(name, size) values('Mars', 'small');"
	);
	s1.step_final();
	SharedSQLStatement s2
	(	dbc,
		"insert into planets(name, size) values('Earth', 'medium');"
	);
	s2.step_final();
	SharedSQLStatement s3
	(	dbc,
		"insert into planets(name, size) values('Venus', 'medium');"
	);
	s3.step_final();

	// Here's the statement we will stuff up the state of
	string const selector_text =
		"select name from planets where size = :pr";
	for (int i = 0; i != 1; ++i)
	{
		SharedSQLStatement selector0(dbc, selector_text);
		CHECK_THROW
		(	selector0.bind(":nonexistentparam", "small"),
			SQLiteException
		);
		// selector0 is now in an invalid state and the underlying
		// SQLStatement has an invalid null pointer as its sqlite3_stmt*
		// member m_statement.
	}
	// But this is still OK
	for (int i = 0; i != 1; ++i)
	{
		SharedSQLStatement selector1(dbc, selector_text);
		selector1.bind(":pr", "small");
		selector1.step();
		string x = selector1.extract<string>(0);
		CHECK_EQUAL(x, "Mars");
	}
}



}  // namespace sqloxx
}  // namespace tests

#include "sqloxx/database_connection.hpp"
#include "sqloxx/shared_sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/tests/sqloxx_tests_common.hpp"

#include <unittest++/UnitTest++.h>
#include <boost/cstdint.hpp>
#include <string>

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

	// Now let's create an unproblematic SQLStatement. This shouldn't throw.
	SharedSQLStatement normal(dbc, "create table dummy(Col_A text)");
}


TEST_FIXTURE(DatabaseConnectionFixture, test_bind_and_extract_normal)
{
	dbc.execute_sql
	(	"create table dummy(Col_A integer primary key autoincrement, "
		"Col_B text not null, Col_C integer not null, Col_D integer)"
	);

	SharedSQLStatement statement_01
	(	dbc,
		"insert into dummy(Col_B, Col_C, Col_D) values(:B, :C, :D)"
	);
	string const hello_01("hello");
	int const x_01(30);
	boost::int64_t y_01(999999983);
	statement_01.bind(":B", hello_01);
	statement_01.bind(":C", x_01);
	statement_01.bind(":D", y_01);
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

	SharedSQLStatement selector
	(	dbc,
		"select Col_B, Col_C, Col_D from dummy where Col_A = 2"
	);
	selector.step();
	CHECK_EQUAL(selector.extract<string>(0), goodbye_02);
	CHECK_EQUAL(selector.extract<int>(1), x_02);
	selector.step_final();
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
	// Test that locking prevents two SharedSQLStatements from
	// sharing the same underlying SQLStatement when they are in
	// the same scope.
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



}  // namespace sqloxx
}  // namespace tests

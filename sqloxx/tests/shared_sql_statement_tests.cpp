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

TEST_FIXTURE(DatabaseConnectionFixture, test_extract_index_exception)
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



// WARNING STILL NEED TO TEST REST OF API



}  // namespace sqloxx
}  // namespace tests

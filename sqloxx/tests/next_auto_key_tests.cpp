#include "sqloxx/database_connection.hpp"
#include "sqloxx/next_auto_key.hpp"
#include "sqloxx/tests/sqloxx_tests_common.hpp"
#include <unittest++/UnitTest++.h>
#include <limits>

using std::numeric_limits;

namespace sqloxx
{
namespace tests
{


TEST(test_next_auto_key_invalid_connection)
{
	DatabaseConnection db0;
	// Have to do this as CHECK_THROW gets confused by multiple template args
	bool ok = false;
	try
	{
		next_auto_key
		<	DatabaseConnection,
			int
		>	(db0, "dummy_table");
	}
	catch (InvalidConnection&)
	{
		ok = true;
	}
	CHECK(ok);
}
	

TEST_FIXTURE(DatabaseConnectionFixture, test_next_auto_key_normal)
{
	// Note CHECK_EQUAL and CHECK get confused by multiple template args
	bool ok =
		(next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	dbc.execute_sql
	(	"create table dummy_table(column_A text)"
	);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	dbc.execute_sql
	(	"create table test_table"
		"("
			"column_A integer not null unique, "
			"column_B integer primary key autoincrement, "
			"column_C text not null"
		")"
	);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 1);
	CHECK(ok);
	// This behaviour is strange but expected - see API docs.
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(3, 'Hello')"
	);
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(4, 'Red')"
	);
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(10, 'Gold')"
	);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 4);
	CHECK(ok);
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "dummy_table") == 1);
	CHECK(ok);
	
	// Test behaviour with gaps in numbering
	dbc.execute_sql("delete from test_table where column_B = 2");
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 4);
	
	// Key is not predicted to be reused once deleted
	dbc.execute_sql("delete from test_table where column_B = 3");
	ok = (next_auto_key<DatabaseConnection, int>(dbc, "test_table") == 4);
	CHECK(ok);
	int const predicted_key =
		next_auto_key<DatabaseConnection, int>(dbc, "test_table");

	// Check key is not actually reused once deleted
	dbc.execute_sql
	(	"insert into test_table(column_A, column_C) "
		"values(110, 'Red')"
	);
	SQLStatement statement2
	(	dbc,
		"select column_B from test_table where column_A = 110"
	);
	statement2.step();
	ok = (statement2.extract<int>(0) == predicted_key);
	CHECK(ok);
	statement2.step_final();

	// Test behaviour in protecting against overflow
	SQLStatement statement
	(	dbc,
		"insert into test_table(column_A, column_B, column_C) "
		"values(:A, :B, :C)"
	);
	statement.bind(":A", 30);
	statement.bind(":B", numeric_limits<int>::max());
	statement.bind(":C", "Hello");
	statement.step_final();

	// Have to do this as CHECK_THROW gets confused by multiple template args
	try
	{
		next_auto_key<DatabaseConnection, int>(dbc, "test_table");
	}
	catch (TableSizeException&)
	{
		ok = true;
	}
	CHECK(ok);
	dbc.execute_sql("drop table dummy_table");
	dbc.execute_sql("drop table test_table");
}



}  // namespace tests
}  // namespace sqloxx

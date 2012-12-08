#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sql_statement.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include "sqloxx/detail/sql_statement_impl.hpp"
#include <unittest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using boost::shared_ptr;
using std::cerr;
using std::endl;
using std::multiset;
using std::ofstream;
using std::multiset;
using std::set;
using std::string;


namespace sqloxx
{
namespace tests
{


// Test DatabaseConnection default constructor, and open function. A one-off
// database file is created and then destroyed in this test.
TEST(test_default_constructor_and_open)
{
	// Define filepaths (without opening)
	boost::filesystem::path const filepath("T55555l2_009873");
	abort_if_exists(filepath);
	boost::filesystem::path const dummy_filepath("lasdy98989823");
	abort_if_exists(dummy_filepath);
	boost::filesystem::path const another_filepath("th887kk899872");
	abort_if_exists(another_filepath);
	boost::filesystem::path const empty_filepath("");
	abort_if_exists(empty_filepath);

	// Test opening new file
	DatabaseConnection dbc;
	CHECK(!dbc.is_valid());
	dbc.open(filepath);  // Note passing filename would also have worked
	CHECK(dbc.is_valid());
	CHECK(file_exists(filepath));

	// Test behaviour when calling open on an existing connection
	CHECK_THROW(dbc.open(filepath), MultipleConnectionException);
	CHECK(file_exists(filepath));

	// Test behaviour when calling open with some other filename corresponding
	// to a file that does not exist.
	CHECK_THROW(dbc.open(dummy_filepath), MultipleConnectionException);
	CHECK(!file_exists(dummy_filepath));
	CHECK(file_exists(filepath));

	// Test behaviour when calling open with some other filename corresponding
	// to a file that does exist, but is not connected to with any database
	// connection
	assert (!file_exists(another_filepath));
	ofstream ifs(another_filepath.string().c_str());
	assert (file_exists(another_filepath));
	CHECK_THROW
	(	dbc.open(another_filepath), MultipleConnectionException
	);
	boost::filesystem::remove(another_filepath);

	// Test opening with an empty string
	DatabaseConnection dbc2;
	CHECK(!dbc2.is_valid());
	CHECK_THROW(dbc2.open(empty_filepath), InvalidFilename);
	CHECK(!file_exists(empty_filepath));

	// Cleanup
	boost::filesystem::remove(filepath);
	assert (!boost::filesystem::exists(boost::filesystem::status(filepath)));
}	

TEST_FIXTURE(DatabaseConnectionFixture, test_is_valid)
{
	CHECK(dbc.is_valid());
	DatabaseConnection dbc2;
	CHECK(!dbc2.is_valid());
}



TEST(test_execute_sql_01)
{
	// Test on unopened DatabaseConnection
	string const command("create table test_table(column_A integer)");
	DatabaseConnection d;
	CHECK_THROW(d.execute_sql(command), InvalidConnection);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_execute_sql_02)
{
	dbc.execute_sql
	(	"create table test_table(column_A integer, column_B text not null)"
	);
	SQLStatement statement_0
	(	dbc,
		"select column_A, column_B from test_table"
	);
	statement_0.step_final();
	dbc.execute_sql
	(	"insert into test_table(column_A, column_B) "
		"values(30, 'Hello')"
	);
	SQLStatement statement_1
	(	dbc,
		"select column_A, column_B from test_table"
	);
	statement_1.step();
	int cell_0 = statement_1.extract<int>(0);
	CHECK_EQUAL(cell_0, 30);
	string cell_1 = statement_1.extract<string>(1);
	CHECK_EQUAL(cell_1, "Hello");
	statement_1.step_final();
	CHECK_THROW
	(	dbc.execute_sql("select mumbo jumbo"),
		SQLiteException
	);
	dbc.execute_sql("drop table test_table");
	CHECK_THROW
	(	dbc.execute_sql("select * from test_table"),
		SQLiteException
	);
}

TEST_FIXTURE(DatabaseConnectionFixture, test_setup_boolean_table)
{
	dbc.setup_boolean_table();
	SQLStatement statement(dbc, "select representation from booleans");
	multiset<int> result;
	while (statement.step())
	{
		result.insert(statement.extract<int>(0));
	}
	CHECK(result.find(0) != result.end());
	CHECK(result.find(1) != result.end());
	CHECK_EQUAL(result.size(), 2);
	CHECK_THROW(dbc.setup_boolean_table(), SQLiteException);
	dbc.execute_sql("drop table booleans");
	DatabaseConnection invaliddb;
	CHECK_THROW(invaliddb.setup_boolean_table(), InvalidConnection);
}

TEST_FIXTURE(DatabaseConnectionFixture, self_test)
{
	// Tests max_nesting()
	CHECK_EQUAL(dbc.self_test(), 0);
}





}  // namespace tests
}  // namespace sqloxx
